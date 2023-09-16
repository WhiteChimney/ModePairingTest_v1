#ifndef TDCQUTAG_H
#define TDCQUTAG_H

#include <QObject>
#include <QWidget>
#include "tdcbase.h"
#include "tdcstartstop.h"
#include "tdcdecl.h"
#include "tdchbt.h"
#include "tdclifetm.h"
#include "tdcmultidev.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QCoreApplication>

#define BUFFER_SIZE 1000000
#define CH_NUM 5

class TdcQutag : public QObject
{
    Q_OBJECT


public:
    explicit TdcQutag(QObject *parent = nullptr);
    ~TdcQutag();

    typedef struct
    {
        Bln32 chEnable[5];
        Int32 chDeadTime[5];
        Int32 chDelay[8];

        TDC_SignalCond chConditioning[5];
        Bln32 chEdge[5];
        double chThreshold[5];

        double timeBase;
        Int32 coincWin;
        Int32 expTime;
    }TdcQutagSettings;

    //---------------------------初始化---------------------------------------------
    int instrInit(int deviceID);//设备初始化
    void calibration();//设备校准

    //-----------------------------TDC数据获取-------------------------------------------
    void getTimeStamps();//获得时间戳
    void clearTimeStamps();//清除时间戳缓存
    void getCoincCounters();//获得符合计数
    void writeTimeStamps(QString fileName, TDC_FileFormat format, qint64 writeTimes);//写入至文件    FORMAT_ASCII, FORMAT_BINARY, FORMAT_COMPRESSED, FORMAT_RAW, FORMAT_NONE
    int GetDeviceType();

    //--------------------------同设置界面交互-----------------------------------------
    void on_setChannelEnable(TdcQutag::TdcQutagSettings settings);//设置通道enable
    void on_setSignalCondition(int ch,TdcQutag::TdcQutagSettings settings);//设置信号触发条件
    void on_setDelay(TdcQutag::TdcQutagSettings settings);//设置内部延时（ps）
    void on_setDeadTime(int ch,TdcQutag::TdcQutagSettings settings);//设置死时间（ps）
    void on_getCurrentSettings(TdcQutag::TdcQutagSettings * settings);//获取当前设置

    //-------------------------------------获取成员变量--------------------------------------
    void on_getTimeStamps(const Int64 ** timeStamps, const Int8 ** channels, Int64 * validNum);
    void on_getCoincData(const Int32 ** coincData, Int64 * coincUpdates);

signals:    
    void sendInfo(bool state, qint32 errorCode, QString info);
    void GetTimeStampsFinished(Int64 m_timeStamps[BUFFER_SIZE], Int8 m_channels[BUFFER_SIZE], Int32 * m_validNum);
    void clearTimeStampsFinished();

private:
    //-----------------------------------------成员变量----------------------------------------
    TdcQutagSettings m_tdcQutagSettings;
    Int64 m_timeStamps[BUFFER_SIZE]; //时间戳缓冲区（以timebase为单位）
    Int8 m_channels[BUFFER_SIZE];	 //通道号缓冲区
    Int32 m_validNum; //在Buffer读取即清零的模式下  会返回这样一个值以指示此次读取获得了多少位有效的数据

    Int32 m_coincData[31];/*符合计数数组{0,1,...,5,1/2,1/3,2/3,1/4,2/4,3/4,1/5,2/5,3/5,4/5,
                 1/2/3,1/2/4,1/3/4,2/3/4,1/2/5,1/3/5,2/3/5,1/4/5,2/4/5,3/4/5,
                 1/2/3/4,1/2/3/5,1/2/4/5,1/3/4/5,2/3/4/5,1/2/3/4/5}*/
    Int32 m_coincUpdates;//符合计数更新数量
};

#endif // TDCQUTAG_H
