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

    // 自定义扫延时

    Spd300Qsky *spad;
    double timeStart, timeStop, timeStep, timeNow;

    AFG3000 *afg;

    QFile *fFile = new QFile();
    QTextStream fStream;


    // 自定义时域符合

    QVector<double> coinHistogramX,
                    coinHistogramY;

    QVector<double> m_histogramY;
    QVector<double> m_histogramX;


    void initializeCustomizedFunctions();

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
                     QVector<double> &inputX1, QVector<double> &inputY1,
                     QVector<double> &inputX2, QVector<double> &inputY2,
                     QVector<double> &inputX3, QVector<double> &inputY3);
    //配置函数
    void setIni();
    void LoadIni();
};
#endif // MAINWINDOW_H
