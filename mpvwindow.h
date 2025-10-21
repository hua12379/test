#ifndef MPVWINDOW_H
#define MPVWINDOW_H

#include <QWidget>
#include<QContextMenuEvent>
class MpvWindow:public QWidget
{
    Q_OBJECT
public:
public:
    explicit MpvWindow(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
signals:
    void playSignal();
    void shotSignal();
    void stopSignal();
    void setSignal();
    void quitSignal();
};

#endif // MPVWINDOW_H
