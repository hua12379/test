//
// Created by zend on 2025/5/14.
//
#ifndef MPVPLAYER_H
#define MPVPLAYER_H
#include <memory>
#include <QString>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include "MpvEventThread.h"
#include"libs/include/mpv/client.h"
#include"player/MpvInterface.h"
#include"MpvStruct.h"


#define MAX_MPV_PLAYERS 16


/**
 * 自定义单实例播放器，支持实时播放、录像文件回话
 * @brief The MyMpvPlayer class
 */
class MpvPlayer: public MpvInterface
{
    Q_OBJECT

private:
    MpvPlayer();
    ~MpvPlayer();
    static MpvPlayer* instance;
    static void wakeup(void *ctx);
    static QMutex mutex;

    //mpv播放器句柄列表，可根据硬件解码能力设置最大值
    MpvHandle mpvHandleArray[MAX_MPV_PLAYERS];
    MpvEventThread* mpvEventThreadArray[MAX_MPV_PLAYERS];

    //查找空闲的mpv句柄
    int getFreeMpvHandle();

    //释放资源
    void releaseMpv(int handle);

    //检查文件路径是否有效
    bool isValidFilePath(QString path);

public:
    //获取单实例
    static MpvPlayer* getInstance();
    //释放单实例资源
    static void releasePlayer();

    //获取mpv句柄内容，不建议使用该方法
    MpvHandle * getMpv(int handle);

    //检查mpv 句柄是否有效
    bool checkMpvHandle(int handle);

    //rtsp视频实时播放
    int startRtspPlay(QString rtspUrl, int64_t wid);

    //停止播放实时视频或回放视频，会自动释放mpv句柄资源
    bool stopPlay(int handle);

    //录像文件回放或远程录像文件回放
    int startPlayback(QString path, int64_t wid);

    //回放暂停或恢复，调一次为暂停，再调一次恢复
    bool pausePlayback(int handle);

    //设置回放速度,0.01-100，只对录像回放有效
    bool setSpeed(int handle, double speed);

    //获取回放总时长，单位为秒
    double getTotalDuration(int handle);

    //获取当前回放进度时间，单位为秒
    double getTimePos(int handle);

    //跳到指定位置回放，只对录像回放有效
    bool seek(int handle, double seconds);

    //获取播放速度,0.01-100
    double getPlaySpeed(int handle);

    //获取播放暂停状态，暂停还是播放中
    bool isPaused(int handle);

    //实时视频或回放截图
    bool screenShot(int handle, QString path);

    //实时视频或回放开始录像，宏辉摄像机录像会失败
    bool startRecord(int handle, QString path);

    //停止录像
    bool stopRecord(int handle);
    //设置实时视频或回放音量

    //停止所有播放，会自动释放资源
    void stopAllPlay();

    //获取视频宽度
    int64_t getVideoWidth(int handle);
    //获取视频高度
    int64_t getVideoHeight(int handle);
    //获取视频编码格式
    QString getVideoCodec(int handle);
    //获取视频帧率，不准确，暂时不要用
    double getVideoFps(int handle);
    //获取码率，不准确，暂时不要用
    double getVideoBitrate(int handle);
    //获取当前视频硬件解码方式，no/false为软件解码，空为获取失败
    QString getVideoHardwareDecoder(int handle);
    //获取当前视频显示输出方式
    QString getVideoVo(int handle);
    //获取音频编码格式
    QString getAudioCodec(int handle);
    //获取音频通道数
    int64_t getAudioChannels(int handle);
    //获取音频采样率
    int64_t getAudioSamplerate(int handle);


private slots:
    void onMpvEvents(int, my_mpv_event_id);
signals:
    void mpv_events(int, my_mpv_event_id);
};

#endif // MPVPLAYER_H
