#ifndef TEKAWG5200_H
#define TEKAWG5200_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QCoreApplication>

#include "TekVisa.h"
#include "visa.h"
#include "visatype.h"
#include "vpptype.h"

namespace Ui {
class TekAWG5200;
}

class TekAWG5200 : public QDialog
{
    Q_OBJECT
public:
    explicit TekAWG5200(QWidget *parent = nullptr);

    //visa基础函数
    int VisaOpen();
    int VisaClose();
    int QueryIDN();
    ViStatus InstrWrite (QString str);
    ViStatus InstrRead (QString cmdStr, QString * retStr);
    ViStatus InstrQueryf(QString cmdStr, QString readFmtStr, QString * retStr);
    //AWG控制函数
    int FindSources();
    int OpenChannel(int chIdx);
    int CloseChannel(int chIdx);
    int ChEnabled (int chIdx);
    int CreatNewWavelist (QString fileName, int pointNum );
    int ComplieBasicWaveForm (QString fileName, QString function, QString autoCalc,
                             QString frequency, QString length, QString samRate, QString cycles,
                             QString amplitude, QString offset);
    int GetList ();
    int SetWaveformToCh(int chIdx, QString waveName);
    int SetSKEW (int chIdx, int delay);
    int GetSKEW (int chIdx);
    int SetMarkerDelay (int chIdx, int markerIdx, int delay);
    int GetMarkerDelay (int chIdx, int markerIdx);
    int WriteWlistAnaData(QString & fileName, QVector<float> & anaData, int beginPos);
    int WriteMarkerData(QString &fileName,QVector<bool> & M1, QVector<bool> & M2,
                        QVector<bool> & M3, QVector<bool> & M4);

    //槽函数
    void on_set_visa_addr(QString str);

    //类型转换
    char * QString2charArr(QString str);
    char * Float2charArr(float ipt, char opt[]);
    char Bools2char(bool M1, bool M2, bool M3, bool M4);

signals:

private:
    Ui::TekAWG5200 *ui;

    ViUInt32 m_maxCnt = 100; //最大读取长度
    QString m_visaAddr = "USB0::0x0699::0x0503::B040956::INSTR"; // 仪器的visa地址
    ViSession m_defaultRM, m_instr; /* Communication channels */
    ViUInt32 m_retCnt;
    ViStatus m_status; /* For checking errors */

    void on_btn_open_visa();
    void on_btn_close_visa();
    void on_btn_query();
    void on_btn_open_channel();
    void on_btn_close_channel();
    void on_btn_test();
};

#endif // TEKAWG5200_H
