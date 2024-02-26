#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <QtMath>
#include <QFile>
#include <QTextStream>

double sum(double *a, int N)
{
    double sumN = 0.0;
    for (int i = 0; i < N; i++)
        sumN += a[i];
    return sumN;
}

double sum(int *a, int N)
{
    double sumN = 0.0;
    for (int i = 0; i < N; i++)
        sumN += a[i];
    return sumN;
}

void MainWindow::startUpCustomizedFunctions()
{
    double binStart = 0.0,
           binEnd = 160000.0,
           binSize = 10.0;

    double binAt = binStart;
    while (binAt < binEnd)
    {
        coinHistogramX.append(binAt);
        binAt += binSize;
    }
    coinHistogramY1.resize(coinHistogramX.size());
    coinHistogramY1.fill(0.0);
    coinHistogramY2.resize(coinHistogramX.size());
    coinHistogramY2.fill(0.0);
    qDebug() << coinHistogramY1.size();
    qDebug() << coinHistogramY1.last();

    totalTimer = new QTimer(this);
    timeCounter = 0;
    connect(totalTimer,&QTimer::timeout,this,[=](){
        ui->lcdTimer->display(timeCounter++);});


    // ***************************************************************
    // 数据路径
    QDir dir;
    ghostDataDir = "D:/Data/20240220/SPAD_1ns_eta0.1_u0.1_XCH=20/";
    // ****************************************************************

    dir.setPath(ghostDataDir);
    if (!dir.exists())
        dir.mkdir(ghostDataDir);
    for (int i = 0; i < ghostXpoints; i++)
    {
        QString ghostDataFile = ghostDataDir + "ghost" + QString::number(i+1) + ".txt";
        QFile *fGhostTemp = new QFile();
        fGhostTemp->setFileName(ghostDataFile);
        fGhost.append(fGhostTemp);
    }

    Ghostnorm_TGI = new double[T]();
    Ghostnorm_TGI_classic = new double[T]();
    Icoin_ave = new double[T]();
    cov_QTGI1 = new double[T]();
    Iref_sum = new int[T]();
    Icoin_sum = new int[T]();

    int index = 0;
    for (int order = 3; order < 7; order++)
    {
        for (int j = 1; j < 10; j++)
        {
            nRefPulses[index] = int(j*pow(10,order));
            index++;
        }
    }
    nRefPulses[ghostXpoints-1] = 10000000;
//    for (int i = 0; i < ghostXpoints; i++)
//        qDebug() << nRefPulses[i];

}

void MainWindow::on_read_time_stamps(Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum)
{
    static double timeN = 0.0;
//    timeN = 0.0;
    timeN++;
//    qDebug() << "ValidNum" << * validNum;
//    QString info = "Valid number is:" + QString::number(* validNum);
//    revInfo(true, 0, info);

//    qDebug() << timeStamps[*validNum-1];
//    qDebug() << channels[*validNum-1];

//    读取时间戳后运行时域鬼成像的代码
//    时间单位为 ps



    // ****************************************************************
    // 时域参数
    int channelStart = 1; // Start 信号的通道号
    int channelSNPD = 2; // 超导信号的通道号
    int channelSPAD = 3; // 门控探测器信号的通道号

//    double sample = 10.0;         // TDC 以 1 ps 的精度读取数据，
//                                  // 我们以 sample ps 取一个数据点
                                  // 问天 SPAD 以 10 ps 精度扫描探测效率曲线
    double Izb = 0.0;                   // 用于表示 start 信号位置
    double T_SN;                  // 用于表示超导信号相对于start信号位置
//    int T = floor(20.0e3 / sample);
//                                   IM 斩波后门宽
    static long long timebin = 0;              // 累计时间内发送的总脉冲数
//    timebin = 0;
//    double delaySN=floor((960.0e3)/sample);
//    double delaySN=floor((295.0e3)/sample);
    double delaySN=floor((42.0e3)/sample);
                                  // 超导信号与时钟信号相对延迟：IDQ:768ns Qasky：830ns

//    int ST = floor(17.0e3 / sample);  // 2ns-SPAD端开门起始点
//    int T0 = floor(6.0e3 / sample); // 时域物体总长度，注意长度要小于斩波宽度
    int ST = floor(5.0e3 / sample);  // 2ns-SPAD端开门起始点
    int T0 = floor(8e3 / sample); // 时域物体总长度，注意长度要小于斩波宽度


    // ******************************************************************





    static int Iref = 0;        // 参考端时域响应位置，为 0 至 T-1
//    static int *Iref_sum = new int[T]();    // 参考端时域响应总和
    static int *Iref_2_sum = new int[T]();  // 参考端时域响应平方和
//    int achannel_3_sum = 0; // 超导信号的总脉冲数

    static int Itest = 0;       // test 端时域响应，为 0 或 1
    static int Itest_sum = 0;
//    Itest_sum = 0;
    static int Itest_2_sum = 0;

//    static int *Icoin_sum = new int[T]();   // 符合时域响应总和


    int aN_sum = 0;
    for (int i = 0; i < T; i++) aN_sum += Iref_sum[i];
//    double *Icoin_ave = new double[T]();                    // 符合计数均值：************存************
    double *Iref_ave = new double[T]();
    double *Iref_2_ave = new double[T]();
    double Itest_ave = double(Itest_sum) / aN_sum;
    double Itest_2_ave = double(Itest_2_sum) / aN_sum;
//    double *cov_QTGI1 = new double[T]();                    // 关联函数：************存************
    double *Var_I_ref = new double[T]();
    double Var_I_test = Itest_2_ave - Itest_ave*Itest_ave;
//    double *Ghostnorm_TGI = new double[T]();
//    double *Ghostnorm_TGI_classic = new double[T]();

    double dataT;  // 时间戳数据
    for (int i = 0; i < *validNum; i++)
    {
//        if (channels[i] > 0 and channels[i] < 4)
//            fStreamOriginal << channels[i] << "\t" << timeStamps[i] << "\n";

        dataT = floor(timeStamps[i] / sample); // 以 sample 精读采集数据
        if (channels[i] == channelStart)
        {
            if (Itest > 0 and Iref > 0)
            {
                Icoin_sum[Iref]++;
            }
            timebin++;
            Izb = dataT;
            Itest = 0;
            Iref = 0;
        }
        if (channels[i] == channelSNPD) // 判断是否为通道3的数据，超导信号
        {
            Iref = 0;
//            achannel_3_sum++;
            T_SN = dataT - Izb - delaySN; // 判断超导信号与时钟信号相对延迟
            if (T_SN > ST and T_SN < (ST + T0)) // 选择符合门宽内的计数
            {
                Iref = T_SN-1;           // 构建参考端时域响应情况
                Iref_sum[Iref]++;
                Iref_2_sum[Iref]++;
            }
            if (indexRef < ghostXpoints)
            {
                if (sum(Iref_sum,T) > nRefPulses[indexRef])
                {
    //                qDebug() << "********************************\n"
    //                         << "当前脉冲数：" << sum(Iref_sum,T)
    //                         << "\n********************************\n";
                    fGhost[indexRef]->open(QIODevice::WriteOnly | QIODevice::Text);
                    fStreamGhost.setDevice(fGhost[indexRef]);
                    fStreamGhost << "Elapsed time: " << timeCounter << " s\n";
                    fStreamGhost << "Time interval: " << sample << " ps\n";
                    fStreamGhost << "Number of reference pulses: " << sum(Iref_sum,T) << "\n";
                    fStreamGhost << "Ghost Image data: \n";
                    fStreamGhost << "Ghostnorm_TGI\tGhostnorm_TGI_classic\tcov_QTGI1\tIcoin_ave\n";
                    for (int i = 0; i < T; i++)
                    {
//                        ************* 存 *****************
                        fStreamGhost << Ghostnorm_TGI[i] << "\t";
                        fStreamGhost << Ghostnorm_TGI_classic[i] << "\t";
                        fStreamGhost << cov_QTGI1[i] << "\t";
                        fStreamGhost << Icoin_ave[i] << "\n";
                    }
                    fGhost[indexRef]->close();

                    indexRef++;
                    qDebug() << "out----------------------" << indexRef;
                    if (indexRef == ghostXpoints)
                    {
                        ui->btnStop->on_clicked();
                        on_btnStop_clicked();
                        qDebug() << "out---------------------- done";
//                        qDebug() << "当前周期内的总脉冲数：" << timebin/timeN
//                                 << "超导单道计数： " << sum(Iref_sum,T)/timeN
//                                 << "\tSPAD 单道计数：" << Itest_sum/timeN
//                                 << "\t符合计数： " << sum(Icoin_sum,T)/timeN;
                    }
                }
            }
        }
        if (channels[i] == channelSPAD) // 判断是否为通道4的数据，SPAD信号
        {
            Itest = 1;                       // 通道4存在，spad端计数为1
            Itest_sum++;
            Itest_2_sum++;
        }
    }

    for (int i = 0; i < T; i++)
    {
        Icoin_ave[i] = double(Icoin_sum[i]) / aN_sum;
        Iref_ave[i] = double(Iref_sum[i]) / aN_sum;
        Iref_2_ave[i] = double(Iref_2_sum[i]) / aN_sum;

//        cov_QTGI1[i] = Icoin_ave[i];
        cov_QTGI1[i] = Icoin_ave[i] - Iref_ave[i] * Itest_ave;
        Var_I_ref[i] = Iref_2_ave[i] - Iref_ave[i] * Iref_ave[i];
        Ghostnorm_TGI[i] = Icoin_ave[i] / sqrt(Var_I_ref[i] * Var_I_test);     // 归一化关联函数：************存************
        Ghostnorm_TGI_classic[i] = cov_QTGI1[i] / sqrt(Var_I_ref[i] * Var_I_test);     // 归一化关联函数：************存************
    }

    static int calcRoundCount = 0;

    if (++calcRoundCount >= ui->roundPicRefresh->value())
    {
        calcRoundCount = 0;

        coinHistogramX.clear();
        coinHistogramY1.clear();
        coinHistogramY2.clear();

        for (int i = ST; i < ST+T0-1; i++)
        {
            coinHistogramX.append((i-ST)*sample);
            coinHistogramY1.append((Ghostnorm_TGI[i]));
            coinHistogramY2.append((Ghostnorm_TGI_classic[i]));
        }
        SimplePlot(coinHistogramX,coinHistogramY1,
                   coinHistogramX,coinHistogramY2);

        //                        qDebug() << "当前周期内的总脉冲数：" << timebin/timeN
        //                                 << "超导单道计数： " << sum(Iref_sum,T)/timeN
        //                                 << "\tSPAD 单道计数：" << Itest_sum/timeN
        //                                 << "\t符合计数： " << sum(Icoin_sum,T)/timeN;

        // ************************************
        // 重复频率
        double freq = 10e6;
        // ************************************

        double ratio = timebin/timeN/freq;
        double sSpad = Itest_sum/timeN/ratio;
        double sSnpd = sum(Iref_sum,T)/timeN/ratio;
        double cCoin = sum(Icoin_sum,T)/timeN/ratio;
        double mu = sSpad*sSnpd/cCoin/freq;
        ui->lcdSpad->display(sSpad);
        ui->lcdSnpd->display(sSnpd);
        ui->lcdCoin->display(cCoin);
        ui->lcdMu->display(mu);
    }

//    delete [] Icoin_ave;
    delete [] Iref_ave;
    delete [] Iref_2_ave;
//    delete [] cov_QTGI1;
    delete [] Var_I_ref;
}

void MainWindow::wrapUpCustomizedFunctions()
{
//    收尾工作

    QFile *fGhost = new QFile();
    fGhost->setFileName(ghostDataDir + "ghost_total.txt");
    fGhost->open(QIODevice::WriteOnly | QIODevice::Text);
    fStreamGhost.setDevice(fGhost);

    fStreamGhost << "Total time elapsed: " << timeCounter << " s\n\n";
    fStreamGhost << "Sample time: " << sample << " ps\n\n";
    fStreamGhost << "Number of reference pulses: " << sum(Iref_sum,T) << "\n";

    fStreamGhost << "SPAD single channel cps: " << ui->lcdSpad->value() << "\n";
    fStreamGhost << "SNSPD single channel cps: " << ui->lcdSnpd->value() << "\n";
    fStreamGhost << "coincidence cps: " << ui->lcdCoin->value() << "\n";
    fStreamGhost << "Average photon number: " << ui->lcdMu->value() << "\n\n";

    fStreamGhost << "Ghost Image data: \n";
    fStreamGhost << "Ghostnorm_TGI\tGhostnorm_TGI_classic\tcov_QTGI1\tIcoin_ave\n";
    for (int i = 0; i < T; i++)
    {
//        ******************** 存另两个数据 *************************
        fStreamGhost << Ghostnorm_TGI[i] << "\t";
        fStreamGhost << Ghostnorm_TGI_classic[i] << "\t";
        fStreamGhost << cov_QTGI1[i] << "\t";
        fStreamGhost << Icoin_ave[i] << "\n";
    }

    fGhost->close();

    qDebug() << "Work is done. ";
}

void MainWindow::on_btnRunTdcConti_clicked()
{
    totalTimer->start(1000);
}

void MainWindow::on_btnStop_clicked()
{
    totalTimer->stop();
}
