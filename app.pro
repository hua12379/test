QT       += core gui  network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
INCLUDEPATH +=/usr/share/doc/mpv
INCLUDEPATH +=/usr/local/include/fastdds
INCLUDEPATH +=/usr/local/include/fastrtps
INCLUDEPATH +=/usr/local/include/fastcdr
#LIBS +=-L/home/kylin/app/libs/lib -lmpv -lfastcdr -lfastrtps
LIBS +=-L/usr/lib/aarch64-linux-gnu -lmpv
LIBS +=-L/usr/local/lib -lfastcdr -lfastrtps
CONFIG += c++11
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CSMXP_V3.cxx \
    CSMXP_V3PubSubTypes.cxx \
    CSMXP_V3Subscriber.cxx \
    LoginDlg.cpp \
    TimeSync.cpp \
    TimeSyncWorker.cpp \
    log/Log.cpp \
    main.cpp \
    mainwindow.cpp \
    mpvwindow.cpp \
    player/MpvEventThread.cpp \
    player/MpvInterface.cpp \
    player/MpvPlayer.cpp \
    playmain.cpp

HEADERS += \
    CSMXP_DDS.h \
    CSMXP_V3.h \
    CSMXP_V3PubSubTypes.h \
    CSMXP_V3Subscriber.h \
    CSMXP_v1_1.h \
    LoginDlg.h \
    TimeSync.h \
    TimeSyncWorker.h \
    log/Log.h \
    mainwindow.h \
    mpvwindow.h \
    player/MpvEventThread.h \
    player/MpvInterface.h \
    player/MpvPlayer.h \
    player/MpvStruct.h \
    playmain.h

FORMS += \
    LoginDlg.ui \
    mainwindow.ui \
    playmain.ui


