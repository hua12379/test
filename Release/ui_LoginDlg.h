/********************************************************************************
** Form generated from reading UI file 'LoginDlg.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDLG_H
#define UI_LOGINDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDlg
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *m_pwd;
    QHBoxLayout *horizontalLayout;
    QPushButton *m_ok;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_cancel;

    void setupUi(QDialog *LoginDlg)
    {
        if (LoginDlg->objectName().isEmpty())
            LoginDlg->setObjectName(QString::fromUtf8("LoginDlg"));
        LoginDlg->resize(466, 411);
        verticalLayout = new QVBoxLayout(LoginDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(LoginDlg);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        m_pwd = new QLineEdit(LoginDlg);
        m_pwd->setObjectName(QString::fromUtf8("m_pwd"));
        m_pwd->setContextMenuPolicy(Qt::NoContextMenu);
        m_pwd->setFrame(true);
        m_pwd->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(m_pwd, 0, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        m_ok = new QPushButton(LoginDlg);
        m_ok->setObjectName(QString::fromUtf8("m_ok"));

        horizontalLayout->addWidget(m_ok);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        m_cancel = new QPushButton(LoginDlg);
        m_cancel->setObjectName(QString::fromUtf8("m_cancel"));

        horizontalLayout->addWidget(m_cancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(LoginDlg);

        QMetaObject::connectSlotsByName(LoginDlg);
    } // setupUi

    void retranslateUi(QDialog *LoginDlg)
    {
        LoginDlg->setWindowTitle(QApplication::translate("LoginDlg", "\347\263\273\347\273\237\351\205\215\347\275\256", nullptr));
        label->setText(QApplication::translate("LoginDlg", "\345\257\206\347\240\201:", nullptr));
        m_ok->setText(QApplication::translate("LoginDlg", "\347\241\256\345\256\232", nullptr));
        m_cancel->setText(QApplication::translate("LoginDlg", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDlg: public Ui_LoginDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDLG_H
