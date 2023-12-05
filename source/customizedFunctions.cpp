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
    coinHistogramY.resize(coinHistogramX.size());
    coinHistogramY.fill(0.0);
    qDebug() << coinHistogramY.size();
    qDebug() << coinHistogramY.last();

    totalTimer = new QTimer(this);
    timeCounter = 0;
    connect(totalTimer,&QTimer::timeout,this,[=](){
        ui->lcdTimer->display(timeCounter++);});

    QDir dir;
    QString ghostDataDir = "D:/Data/20231007/SPAD_1ns_eta0.2/";
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
    for (int i = 0; i < ghostXpoints; i++)
        qDebug() << nRefPulses[i];

// 扫探测器门宽
    timeStart = 4.00;
    timeStop = 11.00;
    timeStep = 0.01;
    timeNow = timeStart;
    spad = new Spd300Qsky(0,this);
    spad->set_delay(timeStart);

    QString originDataDir = "D:/Data/20231008/";
    dir.setPath(originDataDir);
    if (!dir.exists())
        dir.mkdir(originDataDir);
    QString originDataFile = originDataDir + "qsky.txt";

    fOriginal->setFileName(originDataFile);
    fOriginal->open(QIODevice::WriteOnly | QIODevice::Text);
    fStreamOriginal.setDevice(fOriginal);

}

void MainWindow::on_read_time_stamps(Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum)
{
//    static double timeN = 0.0;
//    timeN++;
////    qDebug() << "ValidNum" << * validNum;
////    QString info = "Valid number is:" + QString::number(* validNum);
////    revInfo(true, 0, info);

////    qDebug() << timeStamps[*validNum-1];
////    qDebug() << channels[*validNum-1];

////    读取时间戳后运行时域鬼成像的代码
////    时间单位为 ps

//    int channelStart = 1; // Start 信号的通道号
//    int channelSNPD = 3; // 超导信号的通道号
//    int channelSPAD = 2; // 门控探测器信号的通道号

////    double sample = 10.0;         // TDC 以 1 ps 的精度读取数据，
////                                  // 我们以 sample ps 取一个数据点
//                                  // 问天 SPAD 以 10 ps 精度扫描探测效率曲线
//    double Izb = 0.0;                   // 用于表示 start 信号位置
//    double T_SN;                  // 用于表示超导信号相对于start信号位置
////    int T = floor(20.0e3 / sample);
////                                   IM 斩波后门宽
//    static int timebin = 0;              // 累计时间内发送的总脉冲数
//    double delaySN=floor((790.0e3)/sample);
//                                  // 超导信号与时钟信号相对延迟：IDQ:768ns Qasky：830ns

////    int ST = floor(17.0e3 / sample);  // 2ns-SPAD端开门起始点
////    int T0 = floor(6.0e3 / sample); // 时域物体总长度，注意长度要小于斩波宽度
//    int ST = floor(8.0e3 / sample);  // 2ns-SPAD端开门起始点
//    int T0 = floor(10.0e3 / sample); // 时域物体总长度，注意长度要小于斩波宽度

//    static int Iref = 0;        // 参考端时域响应位置，为 0 至 T-1
//    static int *Iref_sum = new int[T]();    // 参考端时域响应总和
//    static int *Iref_2_sum = new int[T]();  // 参考端时域响应平方和
////    int achannel_3_sum = 0; // 超导信号的总脉冲数

//    int Itest = 0;       // test 端时域响应，为 0 或 1
//    static int Itest_sum = 0;
//    static int Itest_2_sum = 0;

//    static int *Icoin_sum = new int[T]();   // 符合时域响应总和

//    double dataT;  // 时间戳数据
//    for (int i = 0; i < *validNum; i++)
//    {
////        if (channels[i] > 0 and channels[i] < 4)
////            fStreamOriginal << channels[i] << "\t" << timeStamps[i] << "\n";

//        dataT = floor(timeStamps[i] / sample); // 以 sample 精读采集数据
//        if (channels[i] == channelStart)
//        {
//            if (Itest > 0 and Iref > 0)
//            {
//                Icoin_sum[Iref]++;
//            }
//            timebin++;
//            Izb = dataT;
//            Itest = 0;
//            Iref = 0;
//        }
//        if (channels[i] == channelSNPD) // 判断是否为通道3的数据，超导信号
//        {
//            Iref = 0;
////            achannel_3_sum++;
//            T_SN = dataT - Izb - delaySN; // 判断超导信号与时钟信号相对延迟
//            if (T_SN > ST and T_SN < (ST + T0)) // 选择符合门宽内的计数
//            {
//                Iref = T_SN-1;           // 构建参考端时域响应情况
//                Iref_sum[Iref]++;
//                Iref_2_sum[Iref]++;
//            }
//            if (indexRef < ghostXpoints)
//            {
//                if (sum(Iref_sum,T) > nRefPulses[indexRef])
//                {
//    //                qDebug() << "********************************\n"
//    //                         << "当前脉冲数：" << sum(Iref_sum,T)
//    //                         << "\n********************************\n";
//                    fGhost[indexRef]->open(QIODevice::WriteOnly | QIODevice::Text);
//                    fStreamGhost.setDevice(fGhost[indexRef]);
//                    fStreamGhost << "Elapsed time: " << timeCounter << " s\n";
//                    fStreamGhost << "Time interval: " << sample << " ps\n";
//                    fStreamGhost << "Number of reference pulses: " << sum(Iref_sum,T) << "\n";
//                    fStreamGhost << "Ghost Image data: \n";
//                    for (int i = 0; i < T; i++)
//                        fStreamGhost << Ghostnorm_TGI[i] << "\n";
//                    fGhost[indexRef]->close();

//                    indexRef++;
//                    qDebug() << "out----------------------" << indexRef;
//                    if (indexRef == ghostXpoints)
//                    {
//                        ui->btnStop->on_clicked();
//                        on_btnStop_clicked();
//                        qDebug() << "out---------------------- done";
////                        qDebug() << "当前周期内的总脉冲数：" << timebin/timeN
////                                 << "超导单道计数： " << sum(Iref_sum,T)/timeN
////                                 << "\tSPAD 单道计数：" << Itest_sum/timeN
////                                 << "\t符合计数： " << sum(Icoin_sum,T)/timeN;
//                    }
//                }
//            }
//        }
//        if (channels[i] == channelSPAD) // 判断是否为通道4的数据，SPAD信号
//        {
//            Itest = 1;                       // 通道4存在，spad端计数为1
//            Itest_sum++;
//            Itest_2_sum++;
//        }
//    }

//    int aN_sum = 0;
//    for (int i = 0; i < T; i++) aN_sum += Iref_sum[i];
//    double *Icoin_ave = new double[T]();
//    double *Iref_ave = new double[T]();
//    double *Iref_2_ave = new double[T]();
//    double Itest_ave = double(Itest_sum) / aN_sum;
//    double Itest_2_ave = double(Itest_2_sum) / aN_sum;
//    double *cov_QTGI1 = new double[T]();
//    double *Var_I_ref = new double[T]();
//    double Var_I_test = Itest_2_ave - Itest_ave*Itest_ave;
////    double *Ghostnorm_TGI = new double[T]();

//    for (int i = 0; i < T; i++)
//    {
//        Icoin_ave[i] = double(Icoin_sum[i]) / aN_sum;
//        Iref_ave[i] = double(Iref_sum[i]) / aN_sum;
//        Iref_2_ave[i] = double(Iref_2_sum[i]) / aN_sum;

//        cov_QTGI1[i] = Icoin_ave[i];
//        //    % cov_QTGI2=Icoin_ave-Iref_ave.*Itest_ave; %% cov(Iref*Itest)=<Iref*Itest>-<Iref><Itest>
//        Var_I_ref[i] = Iref_2_ave[i] - Iref_ave[i]*Iref_ave[i];
//        Ghostnorm_TGI[i] = cov_QTGI1[i] / sqrt(Var_I_ref[i]*Var_I_test);
//    }

//    coinHistogramX.clear();
//    coinHistogramY.clear();

//    for (int i = ST; i < ST+T0-1; i++)
//    {
//        coinHistogramX.append((i-ST)*sample);
//        coinHistogramY.append((Ghostnorm_TGI[i]));
////        coinHistogramY.append((Iref_ave[i]));
//    }
//    SimplePlot(coinHistogramX,coinHistogramY);

//    delete [] Icoin_ave;
//    delete [] Iref_ave;
//    delete [] Iref_2_ave;
//    delete [] cov_QTGI1;
//    delete [] Var_I_ref;

//    //                        qDebug() << "当前周期内的总脉冲数：" << timebin/timeN
//    //                                 << "超导单道计数： " << sum(Iref_sum,T)/timeN
//    //                                 << "\tSPAD 单道计数：" << Itest_sum/timeN
//    //                                 << "\t符合计数： " << sum(Icoin_sum,T)/timeN;

//    double freq = 1e6;
//    double ratio = timebin/timeN/freq;
//    double sSpad = Itest_sum/timeN/ratio;
//    double sSnpd = sum(Iref_sum,T)/timeN/ratio;
//    double cCoin = sum(Icoin_sum,T)/timeN/ratio;
//    double mu = sSpad*sSnpd/cCoin/freq;
//    ui->lcdSpad->display(sSpad);
//    ui->lcdSnpd->display(sSnpd);
//    ui->lcdCoin->display(cCoin);
//    ui->lcdMu->display(mu);

}

void MainWindow::wrapUpCustomizedFunctions()
{
//    收尾工作
    fOriginal->close();

//    fStreamGhost << "Total time elapsed: " << timeCounter << " s\n\n";
//    fStreamGhost << "Sample time: " << sample << " ps\n\n";
//    fStreamGhost << "Ghost Image data: \n";
//    for (int i = 0; i < T; i++)
//        fStreamGhost << Ghostnorm_TGI[i] << "\n";
//    fGhost->close();

//    for (int i = 0; i < ghostXpoints; i++)
//        fGhost[i]->close();

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
