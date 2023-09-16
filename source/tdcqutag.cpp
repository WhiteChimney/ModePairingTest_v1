#include "tdcqutag.h"

TdcQutag::TdcQutag(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<TdcQutag::TdcQutagSettings>("TdcQutag::TdcQutagSettings");
}

TdcQutag::~TdcQutag()
{
    qDebug() << "release TDC";
    TDC_deInit();
}

//设备初始化连接
int TdcQutag::instrInit(int deviceID)
{
    int errorCode;
    errorCode = TDC_init(deviceID);

    //    calibration();
    if (errorCode==0) errorCode = TDC_setTimestampBufferSize(BUFFER_SIZE);  //设置buffer的大小
    if (errorCode==0) errorCode = TDC_getTimebase(& m_tdcQutagSettings.timeBase); //获取时间基
    if (errorCode==0) errorCode = TDC_getDeviceParams(& m_tdcQutagSettings.coincWin,& m_tdcQutagSettings.expTime);
    if (errorCode==0) errorCode = TDC_getDeadTime(0,& m_tdcQutagSettings.chDeadTime[0]);
    if (errorCode==0) errorCode = TDC_getDeadTime(1,& m_tdcQutagSettings.chDeadTime[1]);
    if (errorCode==0) errorCode = TDC_getDeadTime(2,& m_tdcQutagSettings.chDeadTime[2]);
    if (errorCode==0) errorCode = TDC_getDeadTime(3,& m_tdcQutagSettings.chDeadTime[3]);
    if (errorCode==0) errorCode = TDC_getDeadTime(4,& m_tdcQutagSettings.chDeadTime[4]);
    if (errorCode==0) errorCode = TDC_getChannelDelays(m_tdcQutagSettings.chDelay);

    if (errorCode==0)
    {
        qDebug() << "TDC is connected!";
        qDebug() << "Device time base(s):" << m_tdcQutagSettings.timeBase;
        qDebug() << "Device coincidence window(bin/ps):" << m_tdcQutagSettings.coincWin;
        qDebug() << "Device exposure time(ms):" << m_tdcQutagSettings.expTime;
        qDebug() << "Device dead time (ps):" << m_tdcQutagSettings.chDeadTime[0] << m_tdcQutagSettings.chDeadTime[1] << m_tdcQutagSettings.chDeadTime[2] << m_tdcQutagSettings.chDeadTime[3] << m_tdcQutagSettings.chDeadTime[4];
        qDebug() << "Device channel delay (ps):" << m_tdcQutagSettings.chDelay[0] << m_tdcQutagSettings.chDelay[1] << m_tdcQutagSettings.chDelay[2] << m_tdcQutagSettings.chDelay[3] << m_tdcQutagSettings.chDelay[4]
                 << m_tdcQutagSettings.chDelay[5] << m_tdcQutagSettings.chDelay[6] << m_tdcQutagSettings.chDelay[7];
    }

    if (errorCode==0) emit sendInfo(true, 1 , "TDC is connected!");
    else emit sendInfo(false, errorCode, "TDC is not connected!");

    return errorCode;
}

//设备进行校准
void TdcQutag::calibration()
{
    int errorCode;
    Bln32 active;
    errorCode = TDC_startCalibration();
    if(errorCode != TDC_Ok)
    {
        qDebug() << "TDC calibration false";
        emit sendInfo(false, errorCode, "TDC calibration false!");
    }
    TDC_getCalibrationState(&active);

    QElapsedTimer timer;
    timer.start();
    do
    {
        if(timer.hasExpired(200))
        {
            TDC_getCalibrationState(&active);
        }
        timer.restart();
    }while(active);

    qDebug() << "TDC calibration is finished!";
    emit sendInfo(true, 1 , "TDC calibration is completed!");

}





//----------------------------------------获取TDC数据-------------------------------------------------
//获取tdc返回时间戳信息以及信道信息
void TdcQutag::getTimeStamps()
{
    TDC_getLastTimestamps(1, m_timeStamps, m_channels, & m_validNum);
    //    qDebug() << "validNum = " << m_validNum;
    //    for (int idx = 0; idx < m_validNum; idx++) qDebug() << m_channels[idx] << "  " << m_timeStamps[idx];
    emit GetTimeStampsFinished(m_timeStamps, m_channels, & m_validNum);
}

//清除tdc缓存
void TdcQutag::clearTimeStamps()
{
    TDC_getLastTimestamps(1, m_timeStamps, m_channels, & m_validNum);
    emit clearTimeStampsFinished();
}

//获取符合计数，符合间隔为coincidence window，累计时间为exposure time
void TdcQutag::getCoincCounters()
{
    TDC_getCoincCounters(m_coincData, & m_coincUpdates);
}

//直接将数据写入二进制文件中
void TdcQutag::writeTimeStamps(QString fileName, TDC_FileFormat format, qint64 writeTimes)
{

    char * chrFileName;
    QByteArray byaFileName = fileName.toLatin1(); // must
    chrFileName=byaFileName.data();

    TDC_writeTimestamps(chrFileName, format);//

    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < writeTimes)
        QCoreApplication::processEvents();

    TDC_writeTimestamps(" ", FORMAT_NONE);//

    qDebug() << "write successful！" ;
}

//获取设备类型，0为已连接，1为未连接
int TdcQutag::GetDeviceType()
{
    int type = TDC_getDevType();
    return type;
}


//---------------------------------同前面板设置参数界面交互函数-------------------------------------------------
void TdcQutag::on_setChannelEnable(TdcQutag::TdcQutagSettings settings)
{
    Int32 channelMask = settings.chEnable[0] * 1 + settings.chEnable[1] * 2 +
            settings.chEnable[2] * 4 + settings.chEnable[3] * 8 + settings.chEnable[4] * 16;
    TDC_enableChannels(channelMask);
}

void TdcQutag::on_setSignalCondition(int chIdx, TdcQutag::TdcQutagSettings settings)
{
    TDC_configureSignalConditioning((Int32) chIdx, settings.chConditioning[chIdx],
                                    settings.chEdge[chIdx], settings.chThreshold[chIdx]);
    qDebug() << settings.chConditioning[chIdx] << settings.chEdge[chIdx] << settings.chThreshold[chIdx];
}

void TdcQutag::on_setDelay(TdcQutag::TdcQutagSettings settings)
{
    TDC_setChannelDelays(settings.chDelay);
}

void TdcQutag::on_setDeadTime(int chIdx, TdcQutag::TdcQutagSettings settings)
{
    TDC_setDeadTime((Int32) chIdx, settings.chDeadTime[chIdx]);
}

void TdcQutag::on_getCurrentSettings(TdcQutag::TdcQutagSettings * settings)
{
    TDC_getTimebase(& settings->timeBase); //获取时间基
    TDC_getDeviceParams(& settings->coincWin, & settings->expTime);
    TDC_getDeadTime(0, & settings->chDeadTime[0]);
    TDC_getDeadTime(1, & settings->chDeadTime[1]);
    TDC_getDeadTime(2, & settings->chDeadTime[2]);
    TDC_getDeadTime(3, & settings->chDeadTime[3]);
    TDC_getDeadTime(4, & settings->chDeadTime[4]);
    TDC_getChannelDelays(settings->chDelay);

    TDC_getSignalConditioning(0, & settings->chEdge[0], & settings->chThreshold[0]);
    TDC_getSignalConditioning(1, & settings->chEdge[1], & settings->chThreshold[1]);
    TDC_getSignalConditioning(2, & settings->chEdge[2], & settings->chThreshold[2]);
    TDC_getSignalConditioning(3, & settings->chEdge[3], & settings->chThreshold[3]);
    TDC_getSignalConditioning(4, & settings->chEdge[4], & settings->chThreshold[4]);

    settings->chConditioning[0] = SCOND_MISC;
    settings->chConditioning[1] = SCOND_MISC;
    settings->chConditioning[2] = SCOND_MISC;
    settings->chConditioning[3] = SCOND_MISC;
    settings->chConditioning[4] = SCOND_MISC;

    Int32 channelMask;
    TDC_getChannelsEnabled(& channelMask);

    settings->chEnable[0] = channelMask % 2;
    settings->chEnable[1] = (channelMask >> 1) % 2;
    settings->chEnable[2] = (channelMask >> 2) % 2;
    settings->chEnable[3] = (channelMask >> 3) % 2;
    settings->chEnable[4] = (channelMask >> 4) % 2;

}


//-------------------------------------获取成员变量--------------------------------------
//warning：这里数组返回的是成员变量的地址而非值,使用常量指针，即指针可修改，但值不可修改；值返回的就是值；
// 调用示例：const Int32 * a; Int32 b; m_tdcQutag->TdcQutag::on_getA(&a,&b); a为数组b为数值
void TdcQutag::on_getTimeStamps(const Int64 ** timeStamps, const Int8 ** channels, Int64 * validNum)
{
    * timeStamps = m_timeStamps;
    * channels = m_channels;
    * validNum = m_validNum;
}

void TdcQutag::on_getCoincData(const Int32 ** coincData, Int64 * coincUpdates)
{
    * coincData = m_coincData;
    * coincUpdates = m_coincUpdates;
}
