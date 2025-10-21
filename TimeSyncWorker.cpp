#include "TimeSyncWorker.h"
#include<QDebug>
#include"CSMXP_DDS.h"


TimeSyncWorker::TimeSyncWorker(QObject *parent) : QObject(parent)
{
    qInfo() << "[TimeSyncWorker] Worker created in thread:";
}
TimeSyncWorker::~TimeSyncWorker()
{
    qInfo() << "[TimeSyncWorker] Worker destroyed";
}
void TimeSyncWorker::processUdpData(const QByteArray& data)
{
    qDebug() << "[TimeSyncWorker] Processing UDP data, size:" << data.size();

    QByteArray processedData = data;

    // 处理十六进制字符串
    if (isHexString(processedData)) {
        processedData = hexStringToByteArray(processedData);
        qDebug() << "[TimeSyncWorker] Converted hex string to binary";
    }

    TimePacket packet;
    if (parseTimePacket(processedData, packet)) {
        QDateTime syncedTime(QDate(packet.year, packet.month, packet.day),
                            QTime(packet.hour, packet.minute, packet.second));

        qInfo() << "[TimeSyncWorker] UDP time parsed successfully:" << syncedTime.toString("yyyy-MM-dd hh:mm:ss");
        emit timeParsed(syncedTime);
    } else {
        QString error = "Failed to parse UDP time packet";
        qWarning() << "[TimeSyncWorker]" << error;
        emit parseError(error);
    }
}

void TimeSyncWorker::processDdsData(const std::vector<uint8_t>& data)
{
    qDebug() << "[TimeSyncWorker] Processing DDS data, size:" << data.size();

    if (parseCsmxpMessage(data)) {
        qInfo() << "[TimeSyncWorker] DDS message parsed successfully";
    } else {
        QString error = "Failed to parse DDS/CSMXP message";
        qWarning() << "[TimeSyncWorker]" << error;
        emit parseError(error);
    }
}
bool TimeSyncWorker::parseTimePacket(const QByteArray& data, TimePacket& packet)
{
    if (data.size() < 28) {
        qDebug() << "[TimeSyncWorker] Data too small for time packet:" << data.size();
        return false;
    }

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.constData());
    int offset = 20;

    packet.second = bcdToDec(ptr[offset]);
    packet.minute = bcdToDec(ptr[offset + 1]);
    packet.hour = bcdToDec(ptr[offset + 2]);
    packet.day = bcdToDec(ptr[offset + 4]);
    packet.month = bcdToDec(ptr[offset + 5]);
    packet.year = bcdToDec(ptr[offset + 7]) * 100 + bcdToDec(ptr[offset + 6]);

    // 验证时间
    if (packet.year < 2000 || packet.year > 2100 ||
        packet.month < 1 || packet.month > 12 ||
        packet.day < 1 || packet.day > 31 ||
        packet.hour > 23 || packet.minute > 59 || packet.second > 59) {
        qDebug() << "[TimeSyncWorker] Invalid time values:"
                 << packet.year << "-" << packet.month << "-" << packet.day
                 << packet.hour << ":" << packet.minute << ":" << packet.second;
        return false;
    }

    return true;
}
bool TimeSyncWorker::parseCsmxpMessage(const std::vector<uint8_t>& rawData)
{
    if (rawData.size() < sizeof(CSMXPDDSFrameHeader)) {
        qDebug() << "[TimeSyncWorker] Data too small for CSMXP frame header";
        return false;
    }

    const uint8_t* data = rawData.data();
    const CSMXPDDSFrameHeader* frameHeader = reinterpret_cast<const CSMXPDDSFrameHeader*>(data);

    size_t offset = sizeof(CSMXPDDSFrameHeader);

    qDebug() << "[TimeSyncWorker] CSMXP frame - UnitCount:" << frameHeader->UnitCount
             << "SerialNo:" << frameHeader->SerialNo;

    for (int i = 0; i < frameHeader->UnitCount && offset < rawData.size(); i++) {
        if (offset + sizeof(CSMXPDDSUnitHeader) > rawData.size()) {
            break;
        }

        const CSMXPDDSUnitHeader* unitHeader = reinterpret_cast<const CSMXPDDSUnitHeader*>(data + offset);
        uint16_t unitLength = swapBytes16(unitHeader->UnitLength);
        uint16_t unitSubID = swapBytes16(unitHeader->UnitSubID);

        // 检查时间单元
        if ((unitLength == sizeof(CSMXPDDSUnit_TimeBroadcast) && unitSubID == 0x01FA) ||
            (unitHeader->UnitID == 0xFF && unitSubID == 0xB070)) {

            size_t timeOffset = offset + sizeof(CSMXPDDSUnitHeader);
            if (timeOffset + sizeof(CSMXPDDSUnit_TimePeking) <= rawData.size()) {
                if (parseTimeUnit(data + timeOffset, rawData.size() - timeOffset)) {
                    return true;
                }
            }
        }

        offset += unitLength;
    }

    return false;
}

bool TimeSyncWorker::parseTimeUnit(const uint8_t* data, size_t size)
{
    if (size < sizeof(CSMXPDDSUnit_TimePeking)) {
        return false;
    }

    const CSMXPDDSUnit_TimePeking* timePeking = reinterpret_cast<const CSMXPDDSUnit_TimePeking*>(data);

    int second = bcdToDec(timePeking->Second);
    int minute = bcdToDec(timePeking->Minute);
    int hour = bcdToDec(timePeking->Hour);
    int day = bcdToDec(timePeking->Day);
    int month = bcdToDec(timePeking->Month);
    int year = bcdToDec(timePeking->YearHigh) * 100 + bcdToDec(timePeking->YearLow);

    // 验证时间
    if (year < 2000 || year > 2100 ||
        month < 1 || month > 12 ||
        day < 1 || day > 31 ||
        hour > 23 || minute > 59 || second > 59) {
        return false;
    }

    QDateTime syncedTime(QDate(year, month, day), QTime(hour, minute, second));
    qInfo() << "[TimeSyncWorker] Time unit parsed:" << syncedTime.toString("yyyy-MM-dd hh:mm:ss");
    emit timeParsed(syncedTime);

    return true;
}
// 辅助函数实现
uint8_t TimeSyncWorker::bcdToDec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint16_t TimeSyncWorker::swapBytes16(uint16_t value) {
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}
bool TimeSyncWorker::isHexString(const QByteArray& data) {
    QByteArray testData = data;
    testData.replace("\r", "");
    testData.replace("\n", "");
    testData.replace(" ", "");
    testData.replace("\t", "");

    if (testData.isEmpty() || testData.length() % 2 != 0) {
        return false;
    }

    for (char c : testData) {
        if (!((c >= '0' && c <= '9') ||
              (c >= 'a' && c <= 'f') ||
              (c >= 'A' && c <= 'F'))) {
            return false;
        }
    }
    return true;
}

QByteArray TimeSyncWorker::hexStringToByteArray(const QByteArray& hexString) {
    QByteArray cleanHex = hexString;
    cleanHex.replace("\r", "");
    cleanHex.replace("\n", "");
    cleanHex.replace(" ", "");
    cleanHex.replace("\t", "");

    return QByteArray::fromHex(cleanHex);
}
