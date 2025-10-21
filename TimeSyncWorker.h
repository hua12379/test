#ifndef TIMESYNCWORKER_H
#define TIMESYNCWORKER_H
#include <QObject>
#include <QDateTime>
#include "CSMXP_v1_1.h"


class TimeSyncWorker : public QObject
{
    Q_OBJECT
public:
    explicit TimeSyncWorker(QObject *parent = nullptr);
    ~TimeSyncWorker();

signals:
    void timeParsed(const QDateTime& time);
    void parseError(const QString& error);
public slots:
    // 解析UDP数据
    void processUdpData(const QByteArray& data);

    // 解析DDS数据
    void processDdsData(const std::vector<uint8_t>& data);
private:
    //=========CSMXP UDP解析方法============
    bool parseTimePacket(const QByteArray& data,TimePacket& packet);//解析时间数据包
    //处理16进制字符串
    bool isHexString(const QByteArray& data);
    QByteArray hexStringToByteArray(const QByteArray& hexString);

    //=========CSMXP DDS解析方法============
    bool parseCsmxpMessage(const std::vector<uint8_t>& rawData);
    bool parseTimeUnit(const uint8_t* data,size_t size);

    // 辅助函数
    uint8_t bcdToDec(uint8_t bcd);
    uint16_t swapBytes16(uint16_t value);

};

#endif // TIMESYNCWORKER_H
