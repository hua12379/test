#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "TimeSync.h"
namespace Ui {
class Mainwindow;
}

class Mainwindow : public QDialog
{
    Q_OBJECT

public:
    explicit Mainwindow(QWidget *parent = nullptr);
    ~Mainwindow();

private slots:


private:
    Ui::Mainwindow *ui;
//public:
//    UdpTimeSync* m_udpReceiver;
};

#endif // MAINWINDOW_H
