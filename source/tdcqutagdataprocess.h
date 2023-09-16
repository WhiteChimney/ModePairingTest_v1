#ifndef TDCQUTAGDATAPROCESS_H
#define TDCQUTAGDATAPROCESS_H

#include <QObject>
#include <QFile>
#include <QElapsedTimer>
#include "tdcqutag.h"
#include <QDebug>
#include <cmath>


class TdcQutagDataProcess : public QObject
{
    Q_OBJECT

public:
    explicit TdcQutagDataProcess(QObject *parent = nullptr);

    void on_set_file_path(QString fliePath);
    void on_set_big_delay(int chIdx, Int64 extBigDelay);
    void on_set_small_delay(int chIdx, Int64 extSmallDelay);
    void on_set_period(double startFre, double sysFre);
    void on_set_ext_ch_delay();

    Int64 on_get_number_of_channel(int chIdx);
    void on_get_coinc_of_one_peri(Int64* chIdx1Num, Int64* chIdx2Num, Int64* coincNumOfOnePeri);

    void readTimeStampsFile(QString filePath, Int64 readLen);

    void reshapeTimeStamps(Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum);

    void getCoincOfOnePeri(Int8 chIdx1, Int8 chIdx2, Int64 timeStamps[BUFFER_SIZE],
                           Int8 channels[BUFFER_SIZE],Int32 validNum);

    void getCoincOfPair(Int64 pairLen);
    void getCoincOfPair(Int64 pairLen, Int64 part, Int64 * coincNumArr);

    void getBigPeriOfTSOfCh(int chIdx);
    void getSmallPeriOfTSOfCh(int chIdx);
    void getDistOfPeriNum(int chIdx, QVector<double> & count);
    Int64 getAveOfSmallPeri(int chIdx);
    Int64 getAveOfBigPeri(int chIdx);
    Int64 getModeOfBigPeri(int chIdx);
    void getDelayOfSPD (int chIdx1, int chIdx2);


signals:
    void sendInfo(bool state, qint32 errorCode, QString info);
    void sendReTimeStamps(Int64 m_timeStampsOfCh[CH_NUM][BUFFER_SIZE], int m_numOfTSOfCh[CH_NUM]);
    void sendBigPeriOfTSOfCh(Int64 m_bigPeriOfTSOfCh[CH_NUM][BUFFER_SIZE]);
    void sendSmallPeriOfTSOfCh(Int64 m_smallPeriOfTSOfCh[CH_NUM][BUFFER_SIZE]);
    void sendDistOfPeriNum(QVector<double> & count);
    void sendDistInPeri(QVector<double> & count);

    void sendCoincOfOnePeri(Int64 ch1Num,Int64 ch2Num ,Int64 m_coincNumOfOnePeri);

private:
    QString m_filePath = "E:/QT_EXP/wzh_exp/1_Test/test.txt";

    //周期
    double m_timeBase = 1e-12;
    Int64 m_sysPeriod = 1 / 1e+9 / m_timeBase; // QKD系统一个周期的长度，单位是 MyTDC->timeBase(一般是1ps)
    Int64 m_ch0Period = 1 / 1e+6 / m_timeBase; // start信号的一个周期的长度
    Int64 m_periNum = m_ch0Period / m_sysPeriod;

    // 第一个索引[CH_NUM]代表通道号，第二是[BUFFER_SIZE]是数据索引
    Int64 m_timeStampsOfCh[CH_NUM][BUFFER_SIZE]; //存放通道CHx的时间戳数据的数组，单位是 MyTDC->timeBase(一般是1ps)
    int m_numOfTSOfCh[CH_NUM] = {0,0,0,0,0}; //存放通道CHx的时间戳数量的数组，
    Int64 m_bigPeriOfTSOfCh[CH_NUM][BUFFER_SIZE];// 用于存储数据所处于的大周期
    Int64 m_smallPeriOfTSOfCh[CH_NUM][BUFFER_SIZE];// 用于存储数据在某个大周期中的具体位置

    //存储每轮符合计数情况
    Int64 m_chIdx1Num, m_chIdx2Num;//双通道符合，每个通道的响应数
    Int64 m_coincNumOfOnePeri;//符合计数
    Int64 m_NumOfOnePeri;//响应轮次数量
    Int8 m_coincCondOfOnePeri[BUFFER_SIZE];//每轮的符合情况
    Int64 m_coincPeriOfOnePeri[BUFFER_SIZE];//每轮的周期数

    //存储Pair符合计数情况
    Int64 m_coincNumOfPair;
    Int8 m_coincCondOfPair[BUFFER_SIZE];
    Int64 m_coincPeriOfPair[BUFFER_SIZE];

    // 延时
    Int64 m_extBigDelay[CH_NUM]={0,0,0,0,0}; // 数据处理的外部延时 延时的大周期  每个周期的时间为 SysPeriod
    Int64 m_extSmallDelay[CH_NUM]={0,0,0,0,0}; // 数据处理的外部延时 周期内的小延时 单位是 MyTDC->timeBase(一般是1ps)
    Int64 m_extDelay[CH_NUM]={0,0,0,0,0}; // 数据处理的总外部延时 单位是 MyTDC->timeBase(一般是1ps)

};

#endif // TDCQUTAGDATAPROCESS_H
