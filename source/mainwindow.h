#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QElapsedTimer>
#include <QSettings>
#include "tdcqutagconfig.h"
#include "tdcqutag.h"
#include "tdcqutagdataprocess.h"
#include "typeconversion.h"
#include "lneunit.h"
#include "tekawg5200.h"
#include "auxiliaryfunction.h"
#include "spd300qsky.h"
#include "afg3000.h"

#define ghostXpoints 37

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void revInfo(bool state, qint32 errorCode, QString info);

signals:
    void sendFilePath(QString filePath);
    void sendExtBigDelay(int chIdx,Int64 extBigDelay);
    void sendExtSmallDelay(int chIdx,Int64 extBigDelay);

    void reqDistOfPeriNum(Int64 timeStampsOfCh[CH_NUM][BUFFER_SIZE], int numOfTSOfCh[5]);
    void reqCoincNumOfOnePeri(Int8 chIdx1, Int8 chIdx2, Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 validNum);
    void singTest(Int64 &a);


private slots:
    void on_btnRunTdcConti_clicked();

    void on_btnStop_clicked();

private:
    Ui::MainWindow *ui;
    //---------------------------------成员变量-------------------------------------------------
    QString m_iniName = "profile.ini";
    QString m_filePath = "D:/WZH_QT_Program/measureAfterPulse/afterPulseMeasurement_v2/file/test.txt";

    TdcQutag * m_tdcQutag;
    TdcQutagConfig * m_tdcQutagConfig;
    TdcQutagDataProcess * m_tdcQutagDataProcess;
    TypeConversion * m_typeConversion;
    TekAWG5200 * m_tekAWG5200;
    AuxiliaryFunction * m_auxiliaryFuncition;
    QElapsedTimer * m_timer;
    Bln32 m_status = 0;

    QList<QFile*> fGhost;
//    QFile *fGhost = new QFile();
    QTextStream fStreamGhost;
    QString ghostDataDir;


    // 自定义时域符合
    QTimer *totalTimer;
    QVector<double> coinHistogramX,
                    coinHistogramY1, coinHistogramY2;

    QVector<double> m_histogramY;
    QVector<double> m_histogramX;

    double sample = 10.0;         // TDC 以 1 ps 的精度读取数据，
                                  // 我们以 sample ps 取一个数据点
    int T = floor(20.0e3 / sample);
//                                       IM 斩波后门宽
    double *Ghostnorm_TGI, *Ghostnorm_TGI_classic, *Icoin_ave, *cov_QTGI1;
    int *Iref_sum, *Icoin_sum;
//    int Itest_sum;
//    int timebin;
//    double timeN;
//    double *Iref_ave, *Iref_2_ave, *Var_I_ref;
    int timeCounter;
    int nRefPulses[ghostXpoints] = {0};
    int indexRef = 0;

    void startUpCustomizedFunctions();
    void wrapUpCustomizedFunctions();

    //---------------------------------槽函数-------------------------------------------------
    void on_btn_set_filePath();
    void on_read_time_stamps(Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum);
    void on_reshape_time_stamps(Int64 m_timeStampsOfCh[CH_NUM][BUFFER_SIZE], int m_numOfTSOfCh[5]);

    void on_get_delay_between_spd();
    int on_scan_spd_delay(int tdcCh, int awgCh, int awgMarker);

    void on_status_true();
    void on_btn_set_frequency();
    void on_req_coinc_number_of_one_period();

    void on_btn_test();

    void on_plot_tdc();

    //-----------------------------------------函数-------------------------------------------
    //绘图函数
    void SimplePlot (QVector<double> &inputX, QVector<double> &inputY);
    void SimplePlot (QVector<double> &inputX0, QVector<double> &inputY0,
                     QVector<double> &inputX1, QVector<double> &inputY1);
    void SimplePlot (QVector<double> &inputX0, QVector<double> &inputY0,
                     QVector<double> &inputX1, QVector<double> &inputY1,
                     QVector<double> &inputX2, QVector<double> &inputY2,
                     QVector<double> &inputX3, QVector<double> &inputY3);
    //配置函数
    void setIni();
    void LoadIni();
};
#endif // MAINWINDOW_H
