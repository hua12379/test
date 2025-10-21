#ifndef MPVINTERFACE_H
#define MPVINTERFACE_H

#include <QObject>
#include"MpvStruct.h"

class MpvInterface : public QObject
{
    Q_OBJECT
public:
    explicit MpvInterface(QObject *parent = nullptr);
    virtual ~MpvInterface();
    //获取mpv句柄内容，不建议使用该方法
    virtual MpvHandle * getMpv(int handle)=0;
    //RTSP播放
    virtual int startRtspPlay(QString rtspUrl, int64_t wid) = 0;
    //获取空闲的mpv句柄
    virtual int getFreeMpvHandle() = 0;
    //停止RTSP实时播放
    virtual bool stopPlay(int handle) = 0;
    //截屏
    virtual bool screenShot(int handle, QString path) = 0;
    //检查mpv句柄是否有效
    virtual bool checkMpvHandle(int handle) = 0;
    //开始录制
    virtual bool startRecord(int handle, QString path) = 0;
    //停止录制
    virtual bool stopRecord(int handle) = 0;
    //判断文件路径是否有效
    virtual bool isValidFilePath(QString path) = 0;
    //释放mpv资源
    virtual void releaseMpv(int handle) = 0;

    virtual int startPlayback(QString path, int64_t wid) = 0;

    virtual void onMpvEvents(int handle, my_mpv_event_id event_id) = 0;

    //关闭所有
    virtual void stopAllPlay() = 0;

    virtual bool pausePlayback(int handle) = 0;

    virtual double getTotalDuration(int handle) = 0;

    virtual double getTimePos(int handle) = 0;

    virtual double getPlaySpeed(int handle) = 0;

    virtual bool isPaused(int handle) = 0;
    virtual bool seek(int handle, double seconds) = 0;
    virtual bool setSpeed(int handle, double speed) = 0;
    virtual int64_t getVideoWidth(int handle) = 0;
    virtual int64_t getVideoHeight(int handle) = 0;
    virtual QString getVideoCodec (int handle) = 0;
    virtual QString getVideoHardwareDecoder (int handle) = 0;
    virtual QString getVideoVo (int handle) = 0;
    virtual double  getVideoFps(int handle) = 0;
    virtual double getVideoBitrate(int handle) = 0;
    virtual QString getAudioCodec(int handle) = 0;
    virtual int64_t getAudioChannels(int handle) = 0;
    virtual int64_t getAudioSamplerate(int handle) = 0;
signals:
    void mpv_events(int, my_mpv_event_id);


};

#endif // MPVINTERFACE_H
