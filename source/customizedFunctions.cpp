#include "mainwindow.h"
#include <iostream>

void MainWindow::startUpCustomizedFunctions()
{
//    spad = new Spd300Qsky(1,this);
//    spad->set_delay(1.65);

//    timeStart = 112.0;
//    timeStop = 121.0;
//    timeStep = 0.1;
//    timeNow = timeStart;
//    QString resc = "USB0::0x0699::0x0356::C020232::INSTR";
//    afg = new AFG3000(resc,this);
//    qDebug() << afg->initializeDevice();
//    qDebug() << afg->getDelay(1);
//    qDebug() << afg->sendCommand("SOURce1:PULSe:DELay 20ns");
//    qDebug() << afg->getDelay(1);
//    afg->closeDevice();

//    QString fileName = "D:/Data/20230817/ID201_0.10_2.5ns.txt";
//    fFile->setFileName(fileName);
//    fFile->open(QIODevice::WriteOnly | QIODevice::Text);
//    fStream.setDevice(fFile);
//    fStream << tr("扫描激光器的脉冲宽度 50 ps\n");
//    fStream << tr("重复频率为 5.0 MHz\n");
//    fStream << tr("10 % 探测效率，2.5 ns 门宽\n");
//    fStream << tr("延时（ns）\t计数\n");

    double binStart = 0.0,
           binEnd = 160000.0,
           binSize = 10.0;

    double binAt = binStart;
    while (binAt < binEnd)
    {
        coinHistogramX.append(binAt);
        binAt += binSize;
    }
    coinHistogramY.resize(coinHistogramX.size());
    coinHistogramY.fill(0.0);
    qDebug() << coinHistogramY.size();
    qDebug() << coinHistogramY.last();

}

void MainWindow::on_read_time_stamps(Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum)
{
//    qDebug() << "ValidNum" << * validNum;
//    QString info = "Valid number is:" + QString::number(* validNum);
//    revInfo(true, 0, info);

//    qDebug() << timeStamps[*validNum-1];
//    qDebug() << channels[*validNum-1];

//    读取时间戳后运行时域鬼成像的代码
//    时间单位为 ps

    int channelStart = 1; // Start 信号的通道号
    int channelSNPD = 3; // 超导信号的通道号
    int channelSPAD = 2; // 门控探测器信号的通道号

    double sample = 30.0;         // TDC 以 1 ps 的精度读取数据，
                                  // 我们以 sample ps 取一个数据点
                                  // 问天 SPAD 以 10 ps 精度扫描探测效率曲线
    double Izb = 0.0;                   // 用于表示 start 信号位置
    double T_SN;                  // 用于表示超导信号相对于start信号位置
    int T = floor(40.0e3 / sample);
                                  // IM 斩波后门宽
    int timebin = 0;              // 累计时间内发送的总脉冲数
    double delaySN=floor((780.0e3)/sample);
                                  // 超导信号与时钟信号相对延迟：IDQ:768ns Qasky：830ns

    int ST = floor(17.0e3 / sample);  // 2ns-SPAD端开门起始点
    int T0 = floor(6.0e3 / sample); // 时域物体总长度，注意长度要小于斩波宽度

    static int Iref = 0;        // 参考端时域响应位置，为 0 至 T-1
    static int *Iref_sum = new int[T]();    // 参考端时域响应总和
    static int *Iref_2_sum = new int[T]();  // 参考端时域响应平方和
//    int achannel_3_sum = 0; // 超导信号的总脉冲数

    int Itest = 0;       // test 端时域响应，为 0 或 1
    static int Itest_sum = 0;
    static int Itest_2_sum = 0;

    static int *Icoin_sum = new int[T]();   // 符合时域响应总和

    double dataT;  // 时间戳数据
    for (int i = 0; i < *validNum; i++)
    {
        dataT = floor(timeStamps[i] / sample); // 以 sample 精读采集数据
        if (channels[i] == channelStart)
        {
            if (Itest > 0)
            {
                Icoin_sum[Iref]++;
                Itest = 0;
            }
            timebin++;
            Izb = dataT;
        }
        if (channels[i] == channelSNPD) // 判断是否为通道3的数据，超导信号
        {
//            achannel_3_sum++;
            Iref = 0;
            T_SN = dataT - Izb - delaySN; // 判断超导信号与时钟信号相对延迟
            if (T_SN > ST and T_SN < (ST + T0)) // 选择符合门宽内的计数
            {
                Iref = T_SN-1;           // 构建参考端时域响应情况
                Iref_sum[Iref]++;
                Iref_2_sum[Iref]++;
            }
        }
        if (channels[i] == channelSPAD) // 判断是否为通道4的数据，SPAD信号
        {
            Itest = 1;                       // 通道4存在，spad端计数为1
            Itest_sum++;
            Itest_2_sum++;
        }
    }

    int aN_sum = 0;
    for (int i = 0; i < T; i++) aN_sum += Iref_sum[i];
    double *Icoin_ave = new double[T]();
    double *Iref_ave = new double[T]();
    double *Iref_2_ave = new double[T]();
    double Itest_ave = double(Itest_sum) / aN_sum;
    double Itest_2_ave = double(Itest_2_sum) / aN_sum;
    double *cov_QTGI1 = new double[T]();
    double *Var_I_ref = new double[T]();
    double Var_I_test = Itest_2_ave - Itest_ave*Itest_ave;
    double *Ghostnorm_TGI = new double[T]();

    for (int i = 0; i < T; i++)
    {
        Icoin_ave[i] = double(Icoin_sum[i]) / aN_sum;
        Iref_ave[i] = double(Iref_sum[i]) / aN_sum;
        Iref_2_ave[i] = double(Iref_2_sum[i]) / aN_sum;

        cov_QTGI1[i] = Icoin_ave[i];
        //    % cov_QTGI2=Icoin_ave-Iref_ave.*Itest_ave; %% cov(Iref*Itest)=<Iref*Itest>-<Iref><Itest>
        Var_I_ref[i] = Iref_2_ave[i] - Iref_ave[i]*Iref_ave[i];
        Ghostnorm_TGI[i] = cov_QTGI1[i] / sqrt(Var_I_ref[i]*Var_I_test);
    }

    coinHistogramX.clear();
    coinHistogramY.clear();

    for (int i = ST; i < ST+T0-1; i++)
    {
        coinHistogramX.append((i-ST)*sample);
        coinHistogramY.append((Ghostnorm_TGI[i]));
    }
    SimplePlot(coinHistogramX,coinHistogramY);

//    delete [] Iref_sum;
//    delete [] Iref_2_sum;
//    delete [] Icoin_sum;
    delete [] Icoin_ave;
    delete [] Iref_ave;
    delete [] Iref_2_ave;
    delete [] cov_QTGI1;
    delete [] Var_I_ref;
    delete [] Ghostnorm_TGI;

//        int singleCount;
//        singleCount = m_tdcQutagDataProcess->on_get_number_of_channel(3);
//        qDebug() << "当前延时：" << timeNow;
//        spad->set_delay(timeNow);
//        qDebug() << "single count now: " << singleCount;
//        fStream << QString::number(timeNow) << "\t"
//                << QString::number(singleCount) << "\n";
}

void MainWindow::wrapUpCustomizedFunctions()
{
//    收尾工作
    qDebug() << "Work is done. ";
}
