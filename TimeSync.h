#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QDateTime>
#include <QTimer>
#include <QUdpSocket>
#include <memory>
#include"CSMXP_v1_1.h"
// 前向说明
class CSMXP_V3Subscriber;
class TimeSyncWorker;

//时间同步数据包结构
class TimeSync:public QObject
{
    Q_OBJECT
public:
    explicit TimeSync(QObject* parent = nullptr);
    ~TimeSync();
    //获取单例实例
    static TimeSync* getInstance();
//=========UDP接口============
    bool bindUdpPort(quint16 port);  // 绑定UDP端口，等待接收
    void unbindUdpPort();                    // 解绑UDP端口
//=========DDS接口============
    void subscribeDDS(int domain, const QString& topicName,const QString& ip, int port);
    void unsubscribeDDS();
signals:
//通用信号
    void timeSynsSuccess(const QDateTime& syncedTime);//时间同步成功信号
    void timeSynsFailed(const QString& error);   //时间同步失败信号
    void timeSyncStatusChanged(bool synced);   //时间同步状态改变信号
private slots:
    void handleUdpData();
    void checkSyncTimeout();
    void onTimeParsed(const QDateTime& time);
    void onParseError(const QString& error);
private:
    static TimeSync* instance;//存储单例实例的指针
    static QMutex mutex;
    // 工作线程
    QThread* workerThread;
    TimeSyncWorker* worker;
//UDP成员
    QUdpSocket* udpSocket;
//DDS成员
    std::unique_ptr<CSMXP_V3Subscriber> m_ddsSubscriber;
//定时器
    QTimer* timeoutTimer;//超时检查定时器
//状态
    bool isSynced;//是否同步过
    QDateTime lastSyncTime;//最后同步时的本地时间

//=========DDS私有方法============
//DDS数据回调处理(静态函数，用于CSMXP_V3Subscriber回调）
   static void ddsDataHandler(const std::vector<uint8_t>& data, void* userData);

};

#endif // TIMESYNC_H
