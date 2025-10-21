#include "LoginDlg.h"
#include "ui_LoginDlg.h"
#include<QMessageBox>
#include<QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
LoginDlg::LoginDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
}

LoginDlg::~LoginDlg()
{
    delete ui;
}


void LoginDlg::on_m_ok_clicked()
{
    if(ui->m_pwd->text()=="123456"){
        Mainwindow* w = new Mainwindow();
        w->show();
    }else{
        QMessageBox msg(QMessageBox::Information,"提示","密码错误",QMessageBox::Ok);
        msg.exec();
    }

}

void LoginDlg::on_m_cancel_clicked()
{
    QMessageBox msg(QMessageBox::Question,"提示","真的要退出吗?",QMessageBox::Yes|QMessageBox::No);
    msg.exec();
    if(msg.exec()==QMessageBox::Yes){
        this->close();
    }else{

    }
}
