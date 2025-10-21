#include <QApplication>
#include<clocale>
#include "playmain.h"
#include "log/Log.h"
#include"TimeSync.h"

int main(int argc, char *argv[])
{
    qputenv("LC_NUMERIC","C");
    setlocale(LC_NUMERIC, "C");
    QApplication a(argc, argv);
    //初始化日志系统
    Log log;
    log.install(10,30,"./logs",true);
    //获取时间同步实例
    TimeSync* sync = TimeSync::getInstance();
    sync->bindUdpPort(7756);
    //sync->subscribeDDS(0,"CSMXP_TimeSync","239.255.0.1",7400);
//    QObject::connect(sync,&TimeSync::timeSynsSuccess,[](const QDateTime& time){
//        qDebug() << "========================================";
//        qDebug()<<"时间同步成功";
//        qDebug()<<"同步时间:"<<time.toString("yyyy-MM--dd hh:mm:ss");
//        qDebug() << "========================================";
//    });


    playMain w;
    w.show();//设置窗口全屏播放
    //w.setCursor(Qt::BlankCursor);//隐藏鼠标指针
    return a.exec();
}
