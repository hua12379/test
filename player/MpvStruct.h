//
// Created by zend on 2025/5/14.
//

#ifndef MPVSTRUCT_H
#define MPVSTRUCT_H

#include <QString>
#include <QMetaType>
#include<chrono>
#include"libs/include/mpv/client.h"

/**
 * Mpv播放器句柄
 * @brief The MpvHandle struct
 * @author zend
 */
typedef struct MpvHandle
{
    int64_t wid;
    //path or url
    QString path;
    //播放类型
    bool isPlayback;
    //mpv播放器句柄
    mpv_handle * mpv = nullptr;

    //播放状态
    bool isPlayOk = false;//false为RTSP流

    //记录视频像素
    int video_width = 0;
    int video_height = 0;

    //记录最后成功解码时间
    int timePos = 0;
    std::chrono::steady_clock::time_point lastDecodeTime = std::chrono::steady_clock::now();
}MpvHandle;

typedef enum my_mpv_event_id {
    //视频文件或远程视频加载完成，不一定成功
    MY_MPV_EVENT_PLAY_LOADED,
    //开始播放，不一定成功
    MY_MPV_EVENT_START,
    //视频发生变化，如分辨率
    MY_MPV_EVENT_VIDEO_RECONFIG,
    //播放进度改变，可调用
    MY_MPV_EVENT_TIME_POS_CHANGE,
    //播放文件结束，或远程播放中断
    MY_MPV_EVENT_PLAY_END,
    //手动结束播放
    MY_MPV_EVENT_PLAY_STOP,
    //异常结束播放
    MY_MPV_EVENT_PLAY_ERROR_STOP,
    //异常结束播放
    MY_MPV_EVENT_SHUTDOWN
}my_mpv_event_id;

//Q_DECLARE_METATYPE(my_mpv_event_id)

#endif //MYMPVPLAYERPRO_MYMPVSTRUCT_H
