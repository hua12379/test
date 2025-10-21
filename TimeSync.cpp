#include <vector>
#include<QMetaType>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QDebug>
#include "TimeSync.h"
#include"TimeSyncWorker.h"
#include "CSMXP_V3Subscriber.h"
//声明自定义类型
Q_DECLARE_METATYPE(std::vector<uint8_t>)
TimeSync* TimeSync::instance = nullptr;
QMutex TimeSync::mutex;

TimeSync* TimeSync::getInstance() {
    if (instance == nullptr) {
        QMutexLocker locker(&mutex);
        if (instance == nullptr) {
            instance = new TimeSync();
        }
    }
    return instance;
}
TimeSync::TimeSync(QObject *parent) : QObject(parent) {
    //注册自定义类型
    qRegisterMetaType<std::vector<uint8_t>>("std::vector<uint8_t");
    qInfo() << "[TimeSync] Initializing TimeSync system";
    // 创建工作线程
    workerThread = new QThread(this);
    worker = new TimeSyncWorker();
    worker->moveToThread(workerThread);

    // 连接工作线程信号
    connect(worker, &TimeSyncWorker::timeParsed,
            this, &TimeSync::onTimeParsed, Qt::QueuedConnection);
    connect(worker, &TimeSyncWorker::parseError,
            this, &TimeSync::onParseError, Qt::QueuedConnection);

    // 线程生命周期管理
    connect(workerThread, &QThread::started, [this]() {
        qInfo() << "[TimeSync] Worker thread started, ID:" << workerThread->currentThreadId();
    });
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

    // 启动工作线程
    workerThread->start();

    // 创建UDP socket
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &TimeSync::handleUdpData);

    // 创建超时定时器
    timeoutTimer = new QTimer(this);
    connect(timeoutTimer, &QTimer::timeout, this, &TimeSync::checkSyncTimeout);
    timeoutTimer->setInterval(30000); // 30秒超时

    qInfo() << "[TimeSync] TimeSync system initialized successfully";

}

TimeSync::~TimeSync() {
    qInfo() << "[TimeSync] Shutting down TimeSync system";

    unbindUdpPort();
    unsubscribeDDS();

    // 停止工作线程
    if (workerThread) {
        workerThread->quit();
        if (!workerThread->wait(3000)) {
            qWarning() << "[TimeSync] Worker thread did not finish in time";
            workerThread->terminate();
            workerThread->wait();
        }
    }

    qInfo() << "[TimeSync] TimeSync system shutdown complete";

}

// ========= UDP实现 ============
bool TimeSync::bindUdpPort(quint16 port) {
    if (udpSocket->isOpen()) {
        qWarning() << "[TimeSync] UDP socket already bound";
        return true;
    }

    if (!udpSocket->bind(QHostAddress::AnyIPv4, port)) {
        QString error = QString("Failed to bind UDP socket to port %1").arg(port);
        qCritical() << "[TimeSync]" << error;
        emit timeSynsFailed(error);
        return false;
    }

    qInfo() << "[TimeSync] UDP socket bound to port" << port << "- waiting for time packets";
    timeoutTimer->start();

    return true;
}

void TimeSync::unbindUdpPort() {
    if (udpSocket->isOpen()) {
        udpSocket->close();
        qInfo() << "[TimeSync] UDP socket closed";
    }

    timeoutTimer->stop();
    isSynced = false;
    emit timeSyncStatusChanged(false);
}

void TimeSync::handleUdpData() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray data = datagram.data();

        qDebug() << "[TimeSync] Received UDP datagram from"
                 << datagram.senderAddress().toString()
                 << "size:" << data.size();

        // 发送到工作线程处理
        QMetaObject::invokeMethod(worker, "processUdpData",
                                 Qt::QueuedConnection,
                                 Q_ARG(QByteArray, data));
    }
}

// ========= DDS实现 ============
void TimeSync::subscribeDDS(int domain, const QString& topicName,
                           const QString& ip, int port) {
    qInfo() << "[TimeSync] Starting DDS subscription - Domain:" << domain
            << "Topic:" << topicName << "IP:" << ip << "Port:" << port;

    if (m_ddsSubscriber) {
        qWarning() << "[TimeSync] DDS subscriber already exists";
        return;
    }

    m_ddsSubscriber = std::unique_ptr<CSMXP_V3Subscriber>(new CSMXP_V3Subscriber());

    if (!m_ddsSubscriber->init(domain, topicName.toStdString(),
                              ip.toStdString(), port, 0)) {
        m_ddsSubscriber.reset();
        QString error = "Failed to initialize DDS subscriber";
        qCritical() << "[TimeSync]" << error;
        emit timeSynsFailed(error);
        return;
    }

    m_ddsSubscriber->setDataReceiverFunction(&TimeSync::ddsDataHandler, this);
    timeoutTimer->start();

    qInfo() << "[TimeSync] DDS subscription started successfully";
}

void TimeSync::unsubscribeDDS() {
    if (m_ddsSubscriber) {
        qInfo() << "[TimeSync] Unsubscribing from DDS";
        m_ddsSubscriber->uninit();
        m_ddsSubscriber.reset();
    }

    timeoutTimer->stop();
    isSynced = false;
    emit timeSyncStatusChanged(false);
}

void TimeSync::ddsDataHandler(const std::vector<uint8_t>& data, void* userData) {
    if (!userData || data.empty()) {
        return;
    }

    TimeSync* self = static_cast<TimeSync*>(userData);

    qDebug() << "[TimeSync] DDS data received, size:" << data.size();

    // 发送到工作线程处理
    QMetaObject::invokeMethod(self->worker, "processDdsData",
                             Qt::QueuedConnection,
                             Q_ARG(std::vector<uint8_t>, data));
}

void TimeSync::checkSyncTimeout() {
    if (!isSynced) {
        // 还未首次同步成功，继续等待
        return;
    }

    qint64 elapsed = lastSyncTime.msecsTo(QDateTime::currentDateTime());
    if (elapsed > 30000) {
        qWarning() << "[TimeSync] Sync lost - no update for" << elapsed << "ms";
        isSynced = false;
        emit timeSyncStatusChanged(false);
        emit timeSynsFailed("Time sync timeout");
    }
}

void TimeSync::onTimeParsed(const QDateTime& time) {
    lastSyncTime = time;
    bool wasFirstSync = !isSynced;
    isSynced = true;

    if (wasFirstSync) {
        qInfo() << "[TimeSync] ======== FIRST TIME SYNC SUCCESS ========";
        qInfo() << "[TimeSync] Synced time:" << time.toString("yyyy-MM-dd hh:mm:ss");
        qInfo() << "[TimeSync] =========================================";
        emit timeSyncStatusChanged(true);
    } else {
        qDebug() << "[TimeSync] Time updated:" << time.toString("yyyy-MM-dd hh:mm:ss");
    }

    emit timeSynsSuccess(lastSyncTime);

    // 重置超时定时器
    timeoutTimer->stop();
    timeoutTimer->start();
}

void TimeSync::onParseError(const QString& error) {
    qWarning() << "[TimeSync] Parse error:" << error;
    // 解析错误不一定导致同步失败，可能只是某个包的问题
}


