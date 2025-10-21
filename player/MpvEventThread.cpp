//
// Created by zend on 2025/5/12.
//

#include "MpvEventThread.h"
#include "MpvPlayer.h"


void MpvEventThread::run()
{
    MpvHandle *mpvHandle = MpvPlayer::getInstance()->getMpv(handle);
    while (mpvHandle) {
        if (mpvHandle->mpv == nullptr || !isPlay)
        {
            this->msleep(100);
            continue;
        }
        mpv_event *event = mpv_wait_event(mpvHandle->mpv, 0);
        if (mpvHandle->isPlayOk)
        {
            std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(curTime-mpvHandle->lastDecodeTime);
            //3秒无响无解码则抛出播放异常
            if (elapsed_seconds.count() > 3)
            {
                mpvHandle->isPlayOk = false;
                emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_ERROR_STOP);
                //等待关闭mpv
                waitMpvClose();
            }
        }
        if (event->event_id != MPV_EVENT_NONE)
        {
            //qDebug()<<handle<<" mpv event id:"<<event->event_id;
            switch (event->event_id) {
                case MPV_EVENT_START_FILE:
                {
                    //不一定播放成功
                    qDebug()<<handle<<" mpv 开始播放.";
                    emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_LOADED);
                    break;
                }
                case MPV_EVENT_FILE_LOADED:
                {
                    qDebug()<<handle<<" mpv 播放成功.";
                    emit my_mpv_events(handle, MY_MPV_EVENT_START);
                    mpv_observe_property(mpvHandle->mpv, 0, "time-pos", MPV_FORMAT_STRING);
                    mpvHandle->timePos = 0;
                    mpvHandle->isPlayOk = true;
                    mpvHandle->lastDecodeTime = std::chrono::steady_clock::now();
                    break;
                }
                case MPV_EVENT_PROPERTY_CHANGE: {
                    mpv_event_property *prop = (mpv_event_property *)event->data;
                    //播放进度
                    if (strcmp(prop->name, "time-pos") == 0) {
                        mpvHandle->lastDecodeTime = std::chrono::steady_clock::now();
                        if (prop->format == MPV_FORMAT_STRING) {
                            char *value = *(char **)(prop->data);
                            std::string result = prop->data ? value : "";
                            //qDebug()<<QString::fromStdString(result);
                            double time = std::stod(result);
                            int curTimePos = static_cast<int>(time);//Double.IntValue(std::trunc(time));
                            if (curTimePos != mpvHandle->timePos)
                            {
                                mpvHandle->timePos = curTimePos;
                                emit my_mpv_events(handle, MY_MPV_EVENT_TIME_POS_CHANGE);
                            }

                        }
                    }
                    break;
                }
                case MPV_EVENT_VIDEO_RECONFIG: {
                    // 视频尺寸发生变化
                    emit my_mpv_events(handle, MY_MPV_EVENT_VIDEO_RECONFIG);
//                    int64_t w, h;
//                    if (mpv_get_property(mpv, "dwidth", MPV_FORMAT_INT64, &w) >= 0 &&
//                        mpv_get_property(mpv, "dheight", MPV_FORMAT_INT64, &h) >= 0 &&
//                        w > 0 && h > 0)
//                    {
//
//                    }
                    break;
                }
                case MPV_EVENT_LOG_MESSAGE: {
                    struct mpv_event_log_message *msg = (struct mpv_event_log_message *)event->data;
                    qDebug() <<handle << "mpv log -> [" << msg->prefix << "] " << msg->level << ": " << msg->text;
                    break;
                }
                case MPV_EVENT_SHUTDOWN: {
                    //播放器被关闭
                    mpvHandle->isPlayOk = false;
                    emit my_mpv_events(handle, MY_MPV_EVENT_SHUTDOWN);
                    //等待关闭mpv
                    waitMpvClose();
                    break;
                }
                case MPV_EVENT_END_FILE:
                {
                    mpvHandle->isPlayOk = false;
                    mpv_event_end_file * data = (mpv_event_end_file *) event->data;
                    switch (data->reason) {
                        case MPV_END_FILE_REASON_EOF:
                        {
                            qDebug()<<handle<<" mpv 播放完毕或远程视频中断! ";
                            emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_END);
                            break;
                        }
                        case MPV_END_FILE_REASON_STOP:
                        {
                            qDebug()<<handle<<" mpv 手动停止! ";
                            emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_STOP);
                            break;
                        }
                        case MPV_END_FILE_REASON_QUIT:
                        {
                            qDebug()<<handle<<" mpv 播放器退出!";
                            emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_ERROR_STOP);
                            break;
                        }
                        case MPV_END_FILE_REASON_ERROR:
                        {
                            qDebug()<<handle<<" mpv 错误："<<mpv_error_string(data->error);
                            emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_ERROR_STOP);
                            break;
                        }
                        default:
                        {
                            qDebug()<<handle<<" mpv 未知错误："<<mpv_error_string(data->error);
                            emit my_mpv_events(handle, MY_MPV_EVENT_PLAY_ERROR_STOP);
                        }
                    }
                    //等待关闭mpv
                    waitMpvClose();
                }
                    break;
                default: ;
                    // Ignore uninteresting or unknown events.
            }
        }
        else
        {
            this->msleep(20);
        }
    }
}

MpvEventThread::MpvEventThread(int handle) {
    this->handle = handle;
}

void MpvEventThread::stop() {
    requestInterruption();
    quit();
    wait();
}

void MpvEventThread::waitMpvClose() {
    MpvHandle *mpvHandle = MpvPlayer::getInstance()->getMpv(handle);
    if (mpvHandle == nullptr)
    {
        return;
    }
    int waitCount = 0;
    while(1) {
        this->msleep(20);
        waitCount++;
        if (mpvHandle->mpv == nullptr)
        {
            break;
        }
        //等1秒还没关闭则尝试再次通知关闭
        if (waitCount > 50){
            emit my_mpv_events(handle, MY_MPV_EVENT_SHUTDOWN);
            waitCount = 0;
        }
    }
};
