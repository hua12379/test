#ifndef CSMXP_DDS_H
#define CSMXP_DDS_H
#include<stdint.h>
//CSMXP结构体定义
#pragma pack(push,1)
struct IPAddressDDS //IP地址
{
    uint8_t    Byte1;
    uint8_t    Byte2;
    uint8_t    Byte3;
    uint8_t    Byte4;
};
//CSMXP报文信息单元包头
struct CSMXPDDSFrameHeader
{
    uint16_t    Header[6];
    uint16_t    TotalLength; //整个报文长度字节数
    uint16_t    Reserved;//备用
    IPAddressDDS    SendIP; //
    IPAddressDDS    RecvIP;
    uint8_t    SerialNo;//序列号
    uint8_t    AckNo;//确认号
    uint8_t   Flags;//报文标志
    uint8_t    UnitCount;//信息单元个数
};
//CSMXP报文信息单元头部
struct CSMXPDDSUnitHeader
{
    uint8_t    UnitSN;//信息单元序号
    uint8_t    UnitID;//信息单元标识
    uint16_t    UnitLength;//信息单元长度
    uint32_t   TimeStamp;//时间戳
    uint8_t    Reserved;//保留
    uint8_t    Reserved2;//保留
    uint16_t    UnitSubID;//二级信息单元标识，与信息单元标识一起标识信息类型
};
struct CSMXPDDSUnit_TimePeking{
    uint8_t Second,Minute,Hour,Reserved;
    uint8_t Day,Month,YearLow,YearHigh;
};

struct CSMXPDDSUnit_TimeLocal{
    uint8_t Reserved,Second,Minute,Hour;
    uint8_t Day,Month,YearLow,YearHigh;
};
struct CSMXPDDSUnit_TimeBroadcast {
    CSMXPDDSUnitHeader header;
    CSMXPDDSUnit_TimePeking timePeking;
    CSMXPDDSUnit_TimeLocal timeLocal;
};
#pragma pack(pop)
#endif // CSMXP_DDS_H
