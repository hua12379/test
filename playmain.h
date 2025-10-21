#ifndef PLAYMAIN_H
#define PLAYMAIN_H

#include <QMainWindow>
#include"player/MpvInterface.h"
#include<QTimer>
#include "TimeSync.h"
#include "mpvwindow.h"
class QTextEdit;
namespace Ui {
class playMain;
}
class playMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit playMain(QMainWindow *parent = nullptr);
    ~playMain();
private slots:
    void onPlayVideo();
    void onShotVideo();
    void onStopVideo();
    void onSetVideo();
    void onQuitVideo();
    void handle_mpv_event(int handle,my_mpv_event_id eventId);
private:
    Ui::playMain *ui;
    QTextEdit* log;
  //  QWidget* mpv_container;
    MpvWindow* mpv_container;
    int mpvHandle;
    MpvInterface* mpvPlayer;//MpvInterface类的实例指针mpvPlayer
    bool isRecording;
    void append_log(const QString &text);

};

#endif // PLAYMAIN_H
