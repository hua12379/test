//
// Created by zend on 2025/5/12.
//

#ifndef MPVEVENTTHREAD_H
#define MPVEVENTTHREAD_H

#include<QThread>
#include"libs/include/mpv/client.h"
#include"MpvStruct.h"

class MpvEventThread: public QThread {
Q_OBJECT
public:
    MpvEventThread(int handle);
    void run();
    void stop();
    bool isPlay = false;//事件标志
signals:
    void my_mpv_events(int handle, my_mpv_event_id);
private:
    int handle = -1;

    void waitMpvClose();

};


#endif // MPVEVENTTHREAD_H
