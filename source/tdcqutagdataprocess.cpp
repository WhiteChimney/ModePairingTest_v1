#include "tdcqutagdataprocess.h"

TdcQutagDataProcess::TdcQutagDataProcess(QObject *parent)
    : QObject{parent}
{

}

//获得两个探测器之间的延时差距，在一个系统周期中先找到大延时以及小延时的位置，随后获得延时进行调整。
//应用：使用连续光时无法找到延时，切换为一个低速的脉冲光寻找延时
void TdcQutagDataProcess::getDelayOfSPD (int chIdx1, int chIdx2)
{
    getBigPeriOfTSOfCh(chIdx1);
    getBigPeriOfTSOfCh(chIdx2);
    Int64 aveBigCh1 = getAveOfBigPeri(chIdx1);
    qDebug() << "SPD1: the average of big period is" << aveBigCh1;
    Int64 aveBigCh2 = getAveOfBigPeri(chIdx2);
    qDebug() << "SPD2: the average of big period is" << aveBigCh2;

    getSmallPeriOfTSOfCh(chIdx1);
    getSmallPeriOfTSOfCh(chIdx2);
    Int64 aveSmallCh1 = getAveOfSmallPeri(chIdx1);
    qDebug() << "SPD1: the average of small period is" << aveSmallCh1;
    Int64 aveSmallCh2 = getAveOfSmallPeri(chIdx2);
    qDebug() << "SPD2: the average of small period is" << aveSmallCh2;

    int delay = (aveBigCh1-aveBigCh2)*m_sysPeriod  + (aveSmallCh1 - aveSmallCh2);
    qDebug() << "the delay between SPD1 and SPD2 is" << delay << " timeBase(ps)";

}



//按信道重塑时间戳数组并增加外部延时
void TdcQutagDataProcess::reshapeTimeStamps(Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum)
{
    memset(m_timeStampsOfCh,0,sizeof(m_timeStampsOfCh));//初始化数组
    memset(m_numOfTSOfCh,0,sizeof(m_numOfTSOfCh));

    for (int idx = 0; idx < * validNum; idx ++)
    {
        if (channels[idx] >= 0 && channels[idx] < 5)
        {
            m_timeStampsOfCh[channels[idx]][m_numOfTSOfCh[channels[idx]]] =
                    (timeStamps[idx] + m_extDelay[channels[idx]]) % m_ch0Period;
            m_numOfTSOfCh[channels[idx]] ++;

            //            qDebug() << channels[idx] << "  " << timeStamps[idx];
        }
    }

    emit sendReTimeStamps(m_timeStampsOfCh, m_numOfTSOfCh);
}

//获取单个轮次两个探测器的响应情况，1，2分别为两个探测器单独响应，3为同时响应
void TdcQutagDataProcess::getCoincOfOnePeri(Int8 chIdx1, Int8 chIdx2, Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 validNum)
{
    m_chIdx1Num = 0;
    m_chIdx2Num = 0;
    m_coincNumOfOnePeri = 0;
    m_NumOfOnePeri = 0;
    memset(m_coincCondOfOnePeri,0,sizeof(m_coincCondOfOnePeri));//初始化数组
    memset(m_coincPeriOfOnePeri,0,sizeof(m_coincPeriOfOnePeri));//初始化数组

    Int64 arrIdx = 0;

    if (channels[0] == chIdx1 || channels[0] == chIdx2)
    {
        m_coincCondOfOnePeri[arrIdx] = (channels[0] == chIdx1) ? 1 : 2 ;
        m_coincPeriOfOnePeri[arrIdx] = timeStamps[0] / m_sysPeriod;
        m_chIdx1Num += (channels[0] == chIdx1) ? 1 : 0;
        m_chIdx2Num += (channels[0] == chIdx2) ? 1 : 0;
        arrIdx ++;
    }

    for (Int64 idx = 1; idx < validNum; idx++)
    {
        if ((channels[idx] == chIdx1 || channels[idx] == chIdx2) && (abs(timeStamps[idx] - timeStamps[idx-1]) < m_sysPeriod / 2))
        {
            m_coincCondOfOnePeri[arrIdx-1] = 3;
            m_chIdx1Num += (channels[idx] == chIdx1) ? 1 : 0;
            m_chIdx2Num += (channels[idx] == chIdx2) ? 1 : 0;
            m_coincNumOfOnePeri ++;
        }
        else if (channels[idx] == chIdx1 || channels[idx] == chIdx2)
        {
            m_coincCondOfOnePeri[arrIdx] = (channels[idx] == chIdx1) ? 1 : 2 ;
            m_coincPeriOfOnePeri[arrIdx] = timeStamps[idx] / m_sysPeriod;
            m_chIdx1Num += (channels[idx] == chIdx1) ? 1 : 0;
            m_chIdx2Num += (channels[idx] == chIdx2) ? 1 : 0;
            arrIdx ++;
        }
    }
    m_NumOfOnePeri = arrIdx;

    emit sendCoincOfOnePeri(m_chIdx1Num, m_chIdx2Num, m_coincNumOfOnePeri);
}

void TdcQutagDataProcess::getCoincOfPair(Int64 pairLen)
{
    Int64 arrIdx = 0;
    for(int idx = 1; idx < m_coincNumOfOnePeri; )
    {
        if ((m_coincCondOfOnePeri[idx] == 1 || m_coincCondOfOnePeri[idx] == 2) &&
                (m_coincPeriOfOnePeri[idx] - m_coincPeriOfOnePeri[idx-1]) < pairLen)
        {
            m_coincPeriOfPair[arrIdx] = m_coincPeriOfOnePeri[idx] - m_coincPeriOfOnePeri[idx-1];
            m_coincCondOfPair[arrIdx] = (m_coincCondOfOnePeri[idx] == m_coincCondOfOnePeri[idx-1])? 1:2;
            idx = idx + 2;
            arrIdx ++;
        }
        else
        {
            idx ++;
        }
    }
    m_coincNumOfPair = arrIdx - 1;

}

void TdcQutagDataProcess::getCoincOfPair(Int64 pairLen, Int64 part, Int64 * coincNumArr)
{
    Int64 arrIdx = 0;
    for(int idx = 1; idx < m_coincNumOfOnePeri; )
    {
        if ((m_coincCondOfOnePeri[idx] == 1 || m_coincCondOfOnePeri[idx] == 2) &&
                (m_coincPeriOfOnePeri[idx] - m_coincPeriOfOnePeri[idx-1]) < pairLen)
        {
            m_coincPeriOfPair[arrIdx] = m_coincPeriOfOnePeri[idx] - m_coincPeriOfOnePeri[idx-1];
            m_coincCondOfPair[arrIdx] = (m_coincCondOfOnePeri[idx] == m_coincCondOfOnePeri[idx-1])? 1:2;
            coincNumArr[2*((m_coincPeriOfPair[arrIdx]) / (pairLen/part)) + (m_coincCondOfPair[arrIdx] - 1)] ++;
            idx = idx + 2;
            arrIdx ++;
        }
        else
        {
            idx ++;
        }
    }
    m_coincNumOfPair = arrIdx - 1;
}


void TdcQutagDataProcess::readTimeStampsFile(QString filePath, Int64 readLen)
{
    QFile infile(filePath);
    QDataStream in(& infile);
    in.setByteOrder(QDataStream::LittleEndian);

    QVector<Int64> timeStamps;
    timeStamps.resize(readLen);
    QVector<short> channels;
    channels.resize(readLen);

    if (infile.open(QIODevice::ReadOnly))
    {
        for (int idx = 0; idx < readLen ;idx ++)
        {
            in >> timeStamps[idx] >> channels[idx];
            qDebug() << timeStamps[idx] << channels[idx];
        }

        infile.close();
    }
    else
    {
        qDebug() << "open false";
    }

}

// 计算时间戳数据对应于第几个周期
void TdcQutagDataProcess::getBigPeriOfTSOfCh(int chIdx)
{
    memset(m_bigPeriOfTSOfCh[chIdx],0,sizeof(m_bigPeriOfTSOfCh[chIdx]));//初始化数组
    for (int idx = 0; idx < m_numOfTSOfCh[chIdx]; idx ++ )
    {
        m_bigPeriOfTSOfCh[chIdx][idx] = m_timeStampsOfCh[chIdx][idx] / m_sysPeriod;
        //        qDebug() << chIdx << "  " << m_bigPeriOfTSOfCh[chIdx][idx];
    }
}
//获得大延时的平均值
Int64 TdcQutagDataProcess::getAveOfBigPeri(int chIdx)
{
    Int64 aveNum = 0;
    Int64 ave = 0;

    for (int idx = 0; idx < m_numOfTSOfCh[chIdx]; idx ++)
    {
        ave += m_bigPeriOfTSOfCh[chIdx][idx];
        //        qDebug() << chIdx << "  " << m_bigPeriOfTSOfCh[chIdx][idx];
        aveNum ++;
    }
    if(aveNum != 0) ave = ave / aveNum;
    qDebug() << ave << aveNum;
    return ave;
}
//获得大延时的出现最多的值
Int64 TdcQutagDataProcess::getModeOfBigPeri(int chIdx)
{

}

// 计算时间戳数据对应周期内的位置
void TdcQutagDataProcess::getSmallPeriOfTSOfCh(int chIdx)
{
    memset(m_smallPeriOfTSOfCh[chIdx],0,sizeof(m_smallPeriOfTSOfCh[chIdx]));//初始化数组
    for (int idx = 0; idx < m_numOfTSOfCh[chIdx]; idx ++ )
    {
        m_smallPeriOfTSOfCh[chIdx][idx] = m_timeStampsOfCh[chIdx][idx] % m_sysPeriod;
    }
}
//获得小延时的平均值
Int64 TdcQutagDataProcess::getAveOfSmallPeri(int chIdx)
{
    Int64 aveNum = 0;
    Int64 ave = 0;

    for (int idx = 0; idx < m_numOfTSOfCh[chIdx]; idx ++)
    {
        ave += m_smallPeriOfTSOfCh[chIdx][idx];
        aveNum ++;
    }
    if(aveNum != 0) ave = ave / aveNum;
    return ave;
}

// 计算时间戳在周期中计数的分布(以开始信号为触发)
void TdcQutagDataProcess::getDistOfPeriNum(int chIdx, QVector<double> &count)
{
    count.resize(m_periNum);

    for (int idx=0; idx<m_numOfTSOfCh[chIdx];idx++)
    {
        count[m_bigPeriOfTSOfCh[chIdx][idx]]++;
    }

}


















void TdcQutagDataProcess::on_set_file_path(QString filePath)
{
    m_filePath = filePath;
    QString info = "current file path:" + m_filePath;
    emit sendInfo(true, 0, info);
}

void TdcQutagDataProcess::on_set_period(double startFre, double sysFre)
{
    m_sysPeriod = 1 / sysFre / m_timeBase; // QKD系统一个周期的长度，单位是 MyTDC->timeBase(一般是1ps)
    qDebug() << m_sysPeriod;
    m_ch0Period = 1 / startFre / m_timeBase; // start信号的一个周期的长度
    qDebug() << m_ch0Period;
    m_periNum = m_ch0Period / m_sysPeriod;

    QString info = "current startFre:" + QString::number(startFre) + ";  "
            + "current sysFre:" + QString::number(sysFre) + ";  "
            + "current sysPeri:" + QString::number(m_sysPeriod) + ";  "
            + "current startPeri:" + QString::number(m_ch0Period) + ";  ";
    emit sendInfo(true, 0, info);
}

void TdcQutagDataProcess::on_set_big_delay(int chIdx, Int64 extBigDelay)
{
    m_extBigDelay[chIdx] = extBigDelay;
    on_set_ext_ch_delay();

    QString info = "current big delay (sysPeri):";
    for(int idx=0;idx<CH_NUM;idx++)
    {
        info += " " + QString::number(m_extBigDelay[idx]) + " ";
    }
    emit sendInfo(true, 0, info);
}

void TdcQutagDataProcess::on_set_small_delay(int chIdx,Int64 extSmallDelay)
{
    m_extSmallDelay[chIdx] = extSmallDelay;
    on_set_ext_ch_delay();

    QString info = "current small delay (sysPeri):";
    for(int idx=0;idx<CH_NUM;idx++)
    {
        info += " " + QString::number(m_extBigDelay[idx]) + " ";
    }
    emit sendInfo(true, 0, info);
}

//设置TDC外部总延时
void TdcQutagDataProcess::on_set_ext_ch_delay()
{
    for (int idx=0; idx < CH_NUM; idx++)
    {
        m_extDelay[idx] = m_extBigDelay[idx] * m_sysPeriod + m_extSmallDelay[idx];
    }
}

Int64 TdcQutagDataProcess::on_get_number_of_channel(int chIdx)
{
    return m_numOfTSOfCh[chIdx];
}

void TdcQutagDataProcess::on_get_coinc_of_one_peri(Int64* chIdx1Num, Int64* chIdx2Num, Int64* coincNumOfOnePeri)
{
    *chIdx1Num = m_chIdx1Num;
    *chIdx2Num = m_chIdx2Num;
    *coincNumOfOnePeri = m_coincNumOfOnePeri;
}



