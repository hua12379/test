#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDlg; }
QT_END_NAMESPACE

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    LoginDlg(QWidget *parent = nullptr);
    ~LoginDlg();

private slots:
    void on_m_ok_clicked();

    void on_m_cancel_clicked();

private:
    Ui::LoginDlg *ui;
};
#endif // LOGINDLG_H
