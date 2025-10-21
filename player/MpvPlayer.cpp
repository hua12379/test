#include "MpvPlayer.h"
#include<QDebug>
#include<QFile>
#include<QFileInfo>
#include<QMetaType>
MpvPlayer* MpvPlayer::instance=nullptr;
QMutex MpvPlayer::mutex;

MpvPlayer::MpvPlayer()
{
    qRegisterMetaType<my_mpv_event_id>("my_mpv_event_id");
    for (int i=0; i < MAX_MPV_PLAYERS; i++)
    {
        mpvEventThreadArray[i] = new MpvEventThread(i);
        mpvEventThreadArray[i]->start();
        connect(mpvEventThreadArray[i], SIGNAL(finished()), mpvEventThreadArray[i], SLOT(deleteLater()));
        connect(mpvEventThreadArray[i], SIGNAL(my_mpv_events(int, my_mpv_event_id)), this, SLOT(onMpvEvents(int, my_mpv_event_id)));

    }
}
MpvPlayer::~MpvPlayer(){

}
//获取单实例
MpvPlayer* MpvPlayer::getInstance(){
    mutex.lock();
    if(instance==nullptr){
        instance=new MpvPlayer();
    }
    mutex.unlock();
    return instance;
}
//播放器回调
void MpvPlayer::wakeup(void *ctx)
{
    // This callback is invoked from any mpv thread (but possibly also
    // recursively from a thread that is calling the mpv API). Just notify
    // the Qt GUI thread to wake up (so that it can process events with
    // mpv_wait_event()), and return as quickly as possible.

    if (ctx != nullptr)
    {
        MpvEventThread* mpvEventThread = (MpvEventThread *)ctx;
        mpvEventThread->isPlay = true;
    }
}
int MpvPlayer::startRtspPlay(QString rtspUrl,int64_t wid){
    //检查rtspurl是否为空
    if(rtspUrl.isEmpty()){
        qWarning()<<"rtspUrl is empty.rtspUrl:"<<rtspUrl;
        return -1;
    }
    //检查是否有空的mpv播放器句柄
    int handle=getFreeMpvHandle();
    if(handle==-1){
        qWarning()<<"No Free mpv handle.rtspurl"<<rtspUrl;
        return -1;
    }
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    //创建mpv实例
    mpvHandleArray[handle].mpv=mpv_create();
    if(mpvHandleArray[handle].mpv==nullptr){
        return -1;
    }
    //设置日志级别
    mpv_request_log_messages(mpvHandleArray[handle].mpv, "info");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "msg-level", "all-v");
    //设置控制台输出
    //mpv_set_option_string(mpvHandleArray[handle].mpv, "terminal", "false");
    //设置连接超时
    mpv_set_option_string(mpvHandleArray[handle].mpv,"demuxer-lavf-o","stimeout=3000000:rw_timeout=1000000:rtsp_transport=tcp:buffer_size=131072");
    //设置读流超时
    //mpv_set_option_string(mpvHandleArray[handle].mpv,"demuxer-lavf-o","rw_timeout=10000000");
    //设置低延迟播放
    mpv_set_option_string(mpvHandleArray[handle].mpv, "profile", "low-latency");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "cache-pause", "no");
    //mpv_set_option_string(mpvHandleArray[handle].mpv, "demuxer-lavf-o", "rtsp_transport=tcp");
    //mpv_set_option_string(mpvHandleArray[handle].mpv, "demuxer-lavf-o", "buffer_size=131072");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "demuxer-max-bytes", "524288");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "demuxer-max-back-bytes", "524288");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "cache-secs", "0.1");
    //启用流信息侦测，否则low-latency模式下可能会导致录像失败
    mpv_set_option_string(mpvHandleArray[handle].mpv, "demuxer-lavf-probe-info", "yes");

    //启用自动追帧，当延迟过大时跳过帧
    mpv_set_option_string(mpvHandleArray[handle].mpv, "framedrop", "yes");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "audio-buffer", "0.1");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "speed", "1.01");

    //启用硬解码
    mpv_set_option_string(mpvHandleArray[handle].mpv, "hwdec", "auto");
    //设置渲染方式
    mpv_set_option_string(mpvHandleArray[handle].mpv, "vo", "gpu");

   //禁用默认鼠标事件
    mpv_set_option_string(mpvHandleArray[handle].mpv, "input-default-bindings", "no");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "input-cursor", "no");
    //设置RTSP连接超时（单位：微秒）
    //mpv_set_option_string(mpvHandleArray[handle].mpv, "network-timeout", "3"); //官网说对rtsp来说该参数无效
    mpv_set_option_string(mpvHandleArray[handle].mpv,"osd-level","0");
    //设置视频输出到指定窗口
    mpv_set_option(mpvHandleArray[handle].mpv,"wid",MPV_FORMAT_INT64,&wid);
    //设置回调
    mpv_set_wakeup_callback(mpvHandleArray[handle].mpv, wakeup, mpvEventThreadArray[handle]);
    //初始化mpv
    int ret=mpv_initialize(mpvHandleArray[handle].mpv);
    if(ret < 0){
        qCritical()<<"Initialize mpv error"<<mpv_error_string(ret)<<".rtspurl"<<rtspUrl;
        releaseMpv(handle);
        return -1;
    }
    // 加载媒体文件
    const QByteArray c_filename = rtspUrl.toUtf8();
    const char* args[] = {"loadfile", c_filename.data(), nullptr};
    ret = mpv_command_async(mpvHandleArray[handle].mpv, 0, args);
    if (ret < 0) {
        releaseMpv(handle);
        qCritical()<<"mpv play error:"<<mpv_error_string(ret)<<". rtspUrl:"<< rtspUrl;
        return -1;
    }
    mpvHandleArray[handle].isPlayback = false;
    mpvHandleArray[handle].path = rtspUrl;
    mpvHandleArray[handle].wid = wid;

    std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(curTime-now);
    qDebug()<<"初始化"<<handle<<" mpv耗时："<<elapsed_seconds.count();
    return handle; // 成功

}
//截屏
bool MpvPlayer::screenShot(int handle, QString path){
    if(!checkMpvHandle(handle)){
        qWarning()<<"Invalid handle:"<<handle;
        return false;
    }
    if(path.isEmpty()){
        qWarning()<<"Screen shot path is empty.path:"<<path;
    }
    QFileInfo fileinfo(path);
    if(fileinfo.exists()&&fileinfo.isDir()){
        qWarning()<<"Path is directory,not file path:"<<path;
        return false;
    }
    if(fileinfo.exists()&&!fileinfo.isDir()){
        qWarning()<<"File already exists,will overwrite:"<<path;
    }
    const QByteArray c_filename=path.toUtf8();
    const char* args[]={"screenshot-to-file",c_filename.data(),nullptr};
    int ret=mpv_command(mpvHandleArray[handle].mpv,args);
    if(ret < 0){
        qWarning()<<"mpv screenshot error:"<<mpv_error_string(ret)<<".Error code:"<<ret;
        return false;
    }
    return true;
}
int MpvPlayer::getFreeMpvHandle(){
    for(int i=0;i<MAX_MPV_PLAYERS;i++){
        if(mpvHandleArray[i].mpv==nullptr){
            return i;
        }
    }
    return -1;
}
bool MpvPlayer::stopPlay(int handle) {
    if(!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }
    const char* args[] = {"quit", nullptr, nullptr};
    int ret = mpv_command(mpvHandleArray[handle].mpv, args);
    if (ret < 0) {
        qCritical()<<handle<<" mpv stop play error:"<<mpv_error_string(ret);
        return false;
    }
    //releaseMpv(handle);
    return true;
}

bool MpvPlayer::checkMpvHandle(int handle){
    if(handle < 0||handle >= MAX_MPV_PLAYERS){
        return false;
    }
    if(mpvHandleArray[handle].mpv==nullptr){
        return false;
    }
    return true;
}
/**
 * 开始录制，部分摄像机的rtsp录像会失败
 * @brief MyMpvPlayer::startRecord
 * @param handle mpv句柄
 * @param path 录制保存路径
 * @return
 */
bool MpvPlayer::startRecord(int handle, QString path) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }
    // 检查path是否为空
    if (path.isEmpty()) {
        qWarning()<<"Record save path is empty. path:"<< path;
        return false;
    }
    QFileInfo fileInfo(path);
    if (fileInfo.isDir() || fileInfo.exists())
    {
        qWarning()<<"Record save path is invalid. path:"<< path;
        return false;
    }
    const QByteArray c_filename = path.toUtf8();
    //const char* cmd[]={"stream-record",c_filename.data(),nullptr};
    //int ret=mpv_command(mpvHandleArray[handle].mpv,cmd);
    int ret = mpv_set_property_string(mpvHandleArray[handle].mpv, "stream-record", c_filename.data());
    if(ret < 0)
    {
        qCritical()<<"Record error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return false;
    }

    return true;
}

/**
 * 停止录制
 * @brief MyMpvPlayer::stopRecord
 * @param handle mpv句柄
 * @return
 */
bool MpvPlayer::stopRecord(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }

    int ret = mpv_set_property_string(mpvHandleArray[handle].mpv, "stream-record", "");
    if(ret < 0)
    {
        qCritical()<<"Stop record error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return false;
    }
    return true;
}

/**
 * 判断文件路径是否有效
 * @param path
 * @return
 */
bool MpvPlayer::isValidFilePath(QString path) {
    QFileInfo qFileInfo(path);
    if (path.isEmpty())
    {
        qWarning()<<"path is empty. path:"<< path;
        return false;
    }
    if(qFileInfo.exists())
    {
        qWarning()<<"file is exists. path:"<< path;
        return false;
    }
    if (qFileInfo.isDir())
    {
        qWarning()<<"path is dir. path:"<< path;
        return false;
    }
    return true;
}
/** 根据mpv句柄获取mpv，不建议外部业务调用
 * @param handle mpv句柄
 * @return
 */
MpvHandle *MpvPlayer::getMpv(int handle) {
    if (handle < 0 || handle >= MAX_MPV_PLAYERS)
    {
        //qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return nullptr;
    }
    return &mpvHandleArray[handle];
}

/**
 * 释放mpv资源
 * @param handle mpv句柄
 */
void MpvPlayer::releaseMpv(int handle) {
    if (!checkMpvHandle(handle))
    {
        return;
    }
    mpv_terminate_destroy(mpvHandleArray[handle].mpv);
    mpvHandleArray[handle].mpv = nullptr;
    mpvEventThreadArray[handle]->isPlay = false;
    mpvHandleArray[handle].video_width = 0;
    mpvHandleArray[handle].video_height = 0;

}

int MpvPlayer::startPlayback(QString path, int64_t wid) {
    // 检查path是否为空
    if (path.isEmpty()) {
        qWarning()<<"Video file path is empty. video path:"<< path;
        return -1;
    }

    int handle = getFreeMpvHandle();
    if (handle == -1)
    {
        qWarning()<<"No free mpv handle. video path:"<< path;
        return -1;
    }

    std::setlocale(LC_NUMERIC, "C");
    // 创建 mpv 实例
    mpvHandleArray[handle].mpv = mpv_create();

    if (mpvHandleArray[handle].mpv == nullptr) {
        return -1;
    }

    //设置日志级别
    mpv_request_log_messages(mpvHandleArray[handle].mpv, "info");
    mpv_set_option_string(mpvHandleArray[handle].mpv, "msg-level", "all-v");
    //设置控制台输出
    mpv_set_option_string(mpvHandleArray[handle].mpv, "terminal", "false");

    // 启用硬解码
    mpv_set_option_string(mpvHandleArray[handle].mpv, "hwdec", "auto");
    //设置渲染方式
    mpv_set_option_string(mpvHandleArray[handle].mpv, "vo", "gpu");


    // 设置RTSP连接超时（单位：微秒）
    if(path.startsWith("http"))
    {
        mpv_set_option_string(mpvHandleArray[handle].mpv, "network-timeout", "3"); //官网说对rtsp来说该参数无效
    }
    // 设置视频输出到指定窗口
    mpv_set_option(mpvHandleArray[handle].mpv, "wid", MPV_FORMAT_INT64, &wid);

    mpv_set_wakeup_callback(mpvHandleArray[handle].mpv, wakeup, mpvEventThreadArray[handle]);

    int ret = mpv_initialize(mpvHandleArray[handle].mpv);
    if ( ret < 0)
    {
        qCritical()<<"Initialize mpv error:"<<mpv_error_string(ret)<<". Video file:"<< path;
        releaseMpv(handle);
        return -1;
    }

    // 加载媒体文件
    const QByteArray c_filename = path.toUtf8();
    const char* args[] = {"loadfile", c_filename.data(), nullptr};
    ret = mpv_command_async(mpvHandleArray[handle].mpv, 0, args);
    if ( ret < 0) {
        releaseMpv(handle);
        qCritical()<<"mpv play error:"<<mpv_error_string(ret)<<". Video file:"<< path;
        return -1;
    }
    mpvHandleArray[handle].isPlayback = true;
    mpvHandleArray[handle].path = path;
    mpvHandleArray[handle].wid = wid;

    return handle; // 成功
}

void MpvPlayer::onMpvEvents(int handle, my_mpv_event_id event_id) {
    if(event_id == MY_MPV_EVENT_SHUTDOWN || event_id == MY_MPV_EVENT_PLAY_END
    || event_id == MY_MPV_EVENT_PLAY_STOP || event_id == MY_MPV_EVENT_PLAY_ERROR_STOP)
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

        releaseMpv(handle);

        std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(curTime-now);
        qDebug()<<"关闭"<<handle<<" mpv耗时："<<elapsed_seconds.count();
    }
    if (event_id == MY_MPV_EVENT_START || event_id == MY_MPV_EVENT_VIDEO_RECONFIG)
    {
        mpvHandleArray[handle].video_width = instance->getVideoWidth(handle);
        mpvHandleArray[handle].video_height = instance->getVideoHeight(handle);
    }
//    if (event_id == MY_MPV_EVENT_PLAY_END && !mpvHandleArray[handle].isPlayback)
//    {
//        instance->startRtspPlay(mpvHandleArray[handle].path,mpvHandleArray[handle].wid);
//    }
    emit mpv_events(handle, event_id);
}

/**
 * 关闭所有
 */
void MpvPlayer::stopAllPlay() {
    for(int i=0; i<MAX_MPV_PLAYERS; i++)
    {
        stopPlay(i);
    }
}

bool MpvPlayer::pausePlayback(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }
    if(!mpvHandleArray[handle].isPlayback)
    {
        return false;
    }
    int ret = mpv_command_string(mpvHandleArray[handle].mpv, "cycle pause");
    if(ret < 0)
    {
        qCritical()<<"Pause Video error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return false;
    }
    return true;
}

double MpvPlayer::getTotalDuration(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    double time = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "duration", MPV_FORMAT_DOUBLE, &time);
    if(ret < 0)
    {
        qCritical()<<"Get video duration error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return time;
}

double MpvPlayer::getTimePos(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    double time = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "time-pos", MPV_FORMAT_DOUBLE, &time);
    if(ret < 0)
    {
        qCritical()<<"Get video time position error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return time;
}

double MpvPlayer::getPlaySpeed(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    if(!mpvHandleArray[handle].isPlayback)
    {
        return -1;
    }
    double speed = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "speed", MPV_FORMAT_DOUBLE, &speed);
    if(ret < 0)
    {
        qCritical()<<"Get video speed error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return speed;
}

bool MpvPlayer::isPaused(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }
    int pause = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "pause", MPV_FORMAT_FLAG, &pause);
    if(ret < 0)
    {
        qCritical()<<"Get video play status error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return false;
    }
    return pause != 0;
}

bool MpvPlayer::seek(int handle, double seconds) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }
    if (seconds < 0)
    {
        qWarning()<<"Invalid seek param. handle:"<< handle<<", seconds:"<<seconds;
        return false;
    }
    if(!mpvHandleArray[handle].isPlayback)
    {
        return false;
    }

    char cmd[64] = {0};
    snprintf(cmd, sizeof(cmd), "seek %f absolute", seconds);
    int ret = mpv_command_string(mpvHandleArray[handle].mpv, cmd);
    if(ret < 0)
    {
        qCritical()<<cmd<<" error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return false;
    }
    return true;
}

bool MpvPlayer::setSpeed(int handle, double speed) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return false;
    }
    if (speed <= 0 || speed > 100)
    {
        qWarning()<<"Invalid seek param. handle:"<< handle<<", seconds:"<<speed;
        return false;
    }
    if(!mpvHandleArray[handle].isPlayback)
    {
        return false;
    }

    char cmd[64] = {0};
    snprintf(cmd, sizeof(cmd), "set speed %f", speed);
    int ret = mpv_command_string(mpvHandleArray[handle].mpv, cmd);
    if(ret < 0)
    {
        qCritical()<<cmd<<" error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return false;
    }
    return true;
}

int64_t MpvPlayer::getVideoWidth(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    int64_t width = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "width", MPV_FORMAT_INT64, &width);
    if(ret < 0)
    {
        qCritical()<<"Get video width error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return width;
}

int64_t MpvPlayer::getVideoHeight(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    int64_t height = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "height", MPV_FORMAT_INT64, &height);
    if(ret < 0)
    {
        qCritical()<<"Get video height error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return height;
}

QString MpvPlayer::getVideoCodec(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return QString();
    }
    char* value = mpv_get_property_string(mpvHandleArray[handle].mpv, "video-codec");
    std::string result = value ? value : "";
    if (value)
    {
        mpv_free(value);
    }
    return QString::fromStdString(result);
}

QString MpvPlayer::getVideoHardwareDecoder(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return QString();
    }
    char* value = mpv_get_property_string(mpvHandleArray[handle].mpv, "hwdec-current");
    std::string result = value ? value : "";
    if (value)
    {
        mpv_free(value);
    }
    return QString::fromStdString(result);
}

QString MpvPlayer::getVideoVo(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return QString();
    }
    char* value = mpv_get_property_string(mpvHandleArray[handle].mpv, "vo");
    std::string result = value ? value : "";
    if (value)
    {
        mpv_free(value);
    }
    return QString::fromStdString(result);
}

double MpvPlayer::getVideoFps(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    double fps = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "display-fps", MPV_FORMAT_INT64, &fps);
    if(ret < 0)
    {
        qCritical()<<"Get video fps error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return fps;
}

double MpvPlayer::getVideoBitrate(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    double bitrate = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "video-bitrate", MPV_FORMAT_INT64, &bitrate);
    if(ret < 0)
    {
        qCritical()<<"Get video bitrate error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return bitrate;
}

QString MpvPlayer::getAudioCodec(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return QString();
    }
    char* value = mpv_get_property_string(mpvHandleArray[handle].mpv, "audio-codec");
    std::string result = value ? value : "";
    if (value)
    {
        mpv_free(value);
    }
    return QString::fromStdString(result);
}

int64_t MpvPlayer::getAudioChannels(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    int64_t channels = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "audio-params/channel-count", MPV_FORMAT_INT64, &channels);
    if(ret < 0)
    {
        qCritical()<<"Get audio channels error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return channels;
}

int64_t MpvPlayer::getAudioSamplerate(int handle) {
    if (!checkMpvHandle(handle))
    {
        qWarning()<<"Invalid mpv handle. handle:"<< handle;
        return -1;
    }
    int64_t samplerate = 0;
    int ret = mpv_get_property(mpvHandleArray[handle].mpv, "audio-params/samplerate", MPV_FORMAT_INT64, &samplerate);
    if(ret < 0)
    {
        qCritical()<<"Get audio samplerate error:"<<mpv_error_string(ret)<<". handle:"<< handle;
        return -1;
    }
    return samplerate;
}

void MpvPlayer::releasePlayer() {
    if(instance != nullptr)
    {
        instance->stopAllPlay();
        for (int i=0; i < MAX_MPV_PLAYERS; i++)
        {
            instance->mpvEventThreadArray[i]->stop();
        }
        delete instance;
        instance = nullptr;
    }

}
