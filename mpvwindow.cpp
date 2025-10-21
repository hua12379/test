#include "mpvwindow.h"
#include<QMenu>
#include<QAction>
#include<QDebug>
MpvWindow::MpvWindow(QWidget *parent):QWidget(parent)
{

}
//创建右键菜单栏
void MpvWindow::contextMenuEvent(QContextMenuEvent* event){
    QMenu menu(this);
    QAction* actionPlay=menu.addAction("播放视频");
    QAction*actionShot= menu.addAction("视频截图");
    QAction* actionStop=menu.addAction("停止视频");
    menu.addSeparator();
    QAction* actionSettings=menu.addAction("系统配置");
    menu.addSeparator();
    QAction* actionQuit=menu.addAction("关闭软件");
    QAction* selectedAction=menu.exec(event->globalPos());
    if(selectedAction==actionPlay){
        emit playSignal();
    }
    else if(selectedAction==actionShot){
        emit shotSignal();
    }else if(selectedAction==actionStop){
        emit stopSignal();
    }else if(selectedAction==actionQuit){
        emit quitSignal();
    }else if(selectedAction==actionSettings){
        qDebug()<<"Emitting setSignal";
        emit setSignal();
    }
}
