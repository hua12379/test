#include "playmain.h"
#include "ui_playmain.h"
#include "LoginDlg.h"
#include "ui_logindlg.h"
#include<sstream>
#include<QStatusBar>
#include<QTextEdit>
#include<QDebug>
#include<QFileDialog>
#include<QDateTime>
#include<QTimer>

#include"libs/include/mpv/client.h"
#include"player/MpvPlayer.h"
#include"player/MpvInterface.h"
#include"log/Log.h"

playMain::playMain(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::playMain)
{
    ui->setupUi(this);
    mpvHandle=-1;
    isRecording=false;
    //显示日志或信息
    QMainWindow* log_window=new QMainWindow(this);
    log=new QTextEdit(log_window);//在log_window里面创建一个文本编辑控件，赋给成员变量log
    log->setReadOnly(true);//设置为只读，适合用来显示日志
    log_window->setCentralWidget(log);//将log放进log_window里，使其填满整个窗口的中央区域
    log_window->setWindowTitle("mpv log information");
    log_window->setMaximumSize(1024,1080);
    log_window->show();

// 强制这个控件自身创建一个原生窗口句柄wid
  //  mpv_container=new QWidget(this);
    mpv_container = new MpvWindow(this);
    connect(mpv_container,SIGNAL(playSignal()),this,SLOT(onPlayVideo()));
    connect(mpv_container,SIGNAL(shotSignal()),this,SLOT(onShotVideo()));
    connect(mpv_container,SIGNAL(stopSignal()),this,SLOT(onStopVideo()));
    connect(mpv_container,SIGNAL(setSignal()),this,SLOT(onSetVideo()));
    connect(mpv_container,SIGNAL(quitSignal()),this,SLOT(onQuitVideo()));
    setCentralWidget(mpv_container);
    mpv_container->setAttribute(Qt::WA_DontCreateNativeAncestors);
    mpv_container->setAttribute(Qt::WA_NativeWindow);
    mpv_container->setObjectName("mpv_container");
    mpvPlayer = MpvPlayer::getInstance();
    connect(mpvPlayer, SIGNAL(mpv_events(int, my_mpv_event_id)), this, SLOT(handle_mpv_event(int, my_mpv_event_id)));

}

void playMain::handle_mpv_event(int handle, my_mpv_event_id eventId)
{
    QString pw = QString("").setNum(mpv_container->width());
    QString ph = QString("").setNum(mpv_container->height());

    QString sz = pw + QString(":") + ph;
    append_log(sz);
    MpvHandle * mpvHandle = mpvPlayer->getMpv(handle);
    switch (eventId) {
        case MY_MPV_EVENT_PLAY_LOADED:
        {
            std::stringstream ss;
            ss << "开始播放! \n";
            append_log(QString::fromStdString(ss.str()));
            break;
        }
        case MY_MPV_EVENT_START:
        {
            std::stringstream ss;
            ss << "播放成功! \n";
            append_log(QString::fromStdString(ss.str()));
            QString str ;
            str.append("视频宽:" + QString::number(mpvPlayer->getVideoWidth(handle)) );
            str.append(", 高:" + QString::number(mpvPlayer->getVideoHeight(handle)));
            str.append("，视频格式:" + mpvPlayer->getVideoCodec(handle));
            str.append("，视频硬解:" + mpvPlayer->getVideoHardwareDecoder(handle));
            str.append("，视频显示输出:" + mpvPlayer->getVideoVo(handle));
            str.append("\n音频格式：" + mpvPlayer->getAudioCodec(handle));
            str.append(", 音频通道数：" + QString::number(mpvPlayer->getAudioChannels(handle)));
            str.append(", 音频采样率：" + QString::number(mpvPlayer->getAudioSamplerate(handle)));
            statusBar()->showMessage(str);
            str.append("\n视频总时长：" + QString::number(mpvPlayer->getTotalDuration(handle)) + "秒");
            str.append("\n播放速度：" + QString::number(mpvPlayer->getPlaySpeed(handle)));
            str.append("\n");
            append_log(str);
            break;
        }
        case MY_MPV_EVENT_TIME_POS_CHANGE: {
            std::stringstream ss;
            ss << "播放进度:"<<mpvPlayer->getTimePos(handle)<<",总时长："<<mpvPlayer->getTotalDuration(handle)<<"\n";
            append_log(QString::fromStdString(ss.str()));
            break;
        }
        case MY_MPV_EVENT_VIDEO_RECONFIG: {
            QString ss ;
            ss.append("视频宽:" + QString::number(mpvPlayer->getVideoWidth(handle)) );
            ss.append(", 高:" + QString::number(mpvPlayer->getVideoHeight(handle)));
            ss.append("，视频格式:" + mpvPlayer->getVideoCodec(handle));
            ss.append(", 音频格式：" + mpvPlayer->getAudioCodec(handle));
            ss.append(", 音频通道数：" + QString::number(mpvPlayer->getAudioChannels(handle)));
            ss.append(", 音频采样率：" + QString::number(mpvPlayer->getAudioSamplerate(handle)));
            statusBar()->showMessage(ss);
            break;
        }
        case MY_MPV_EVENT_SHUTDOWN: {
            std::stringstream ss;
            ss << "视频播放器关闭\n";
            append_log(QString::fromStdString(ss.str()));
            break;
        }
        case MY_MPV_EVENT_PLAY_END:
            {
                std::stringstream ss;
                ss << "播放完毕或远程视频中断!\n";
                append_log(QString::fromStdString(ss.str()));
                break;
            }
        case MY_MPV_EVENT_PLAY_STOP:
        {
            std::stringstream ss;
            ss << "手动停止播放!\n";
            append_log(QString::fromStdString(ss.str()));
            break;
        }
        case MY_MPV_EVENT_PLAY_ERROR_STOP:
        {
            std::stringstream ss;
            ss << "播放异常停止\n";
            append_log(QString::fromStdString(ss.str()));
            break;
        }
        default: ;
            // Ignore uninteresting or unknown events.
    }
}

playMain::~playMain(){
    //清理mpv资源
    if(mpvHandle!=-1){
        mpvPlayer->releaseMpv(mpvHandle);
        //mpvHandle=-1;
    }
    delete ui;
}
void playMain::onPlayVideo(){
    //进行窗口绑定
    int64_t wid=mpv_container->winId();
    mpvHandle=mpvPlayer->startRtspPlay("rtsp://admin:1234abcd@192.168.1.205:554/h264/ch1/main/av_stream",wid);
    //mpvHandle = mpvPlayer->startRtspPlay("rtsp://admin:1234abcd@192.168.1.107/sub/av_stream", wid);
}
void playMain::append_log(const QString &text)
{
    QTextCursor cursor = log->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text);
    log->setTextCursor(cursor);
}
//实现截图功能
void playMain:: onShotVideo(){
    if(mpvHandle >= 0)
    {
        QString dirname = QFileDialog::getExistingDirectory(this, "Select path to save",QDir::homePath(),QFileDialog::ShowDirsOnly);
        if(dirname.isEmpty())
        {
            return;
        }
        QDateTime current = QDateTime::currentDateTime();
        dirname += "/test" + current.toString("yyyyMMdd_hhmmsszzz") + ".jpg";
        bool ret = mpvPlayer->screenShot(mpvHandle, dirname);
    }
}
void playMain::onStopVideo(){
    if(mpvHandle==-1){
      qDebug()<<"没有正在播放的视频";
      return;
    }
    bool ret=mpvPlayer->stopPlay(mpvHandle);
        if(ret){
            qDebug()<<"视频已停止播放";
            mpvHandle=-1;
        }else{
            qWarning()<<"视频播放失败";
        }
}
void playMain::onSetVideo(){
    qDebug()<<"系统设置";
    LoginDlg* dlg=new LoginDlg(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
    dlg->raise();

}
void playMain::onQuitVideo(){
    if(mpvHandle >= 0){
        mpvPlayer->stopPlay(mpvHandle);
    }
//    //释放整个单例和资源
//    MpvPlayer::releasePlayer();
    this->close();
}





