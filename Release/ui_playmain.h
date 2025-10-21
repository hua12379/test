/********************************************************************************
** Form generated from reading UI file 'playmain.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYMAIN_H
#define UI_PLAYMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_playMain
{
public:

    void setupUi(QWidget *playMain)
    {
        if (playMain->objectName().isEmpty())
            playMain->setObjectName(QString::fromUtf8("playMain"));
        playMain->resize(859, 569);

        retranslateUi(playMain);

        QMetaObject::connectSlotsByName(playMain);
    } // setupUi

    void retranslateUi(QWidget *playMain)
    {
        playMain->setWindowTitle(QApplication::translate("playMain", "\345\233\275\351\230\262\347\233\221\346\216\247\344\270\273\347\224\273\351\235\242 ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class playMain: public Ui_playMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYMAIN_H
