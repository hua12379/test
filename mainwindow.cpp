#include "mainwindow.h"
#include "ui_mainwindow.h"
Mainwindow::Mainwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Mainwindow)
{
    ui->setupUi(this);
    this->setWindowTitle("系统配置");

//    m_udpReceiver = new UdpTimeSync(this);
//    m_udpReceiver->startTimeSync("192.168.1.79",7755);
}

Mainwindow::~Mainwindow()
{
    delete ui;
}

