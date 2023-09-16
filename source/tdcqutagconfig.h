#ifndef TDCQUTAGCONFIG_H
#define TDCQUTAGCONFIG_H

#include <QDialog>
#include "tdcbase.h"
#include "tdcqutag.h"

namespace Ui {
class TdcQutagConfig;
}

class TdcQutagConfig : public QDialog
{
    Q_OBJECT

public:
    explicit TdcQutagConfig(QWidget *parent = nullptr);
    ~TdcQutagConfig();

//---------------------------面板写入与读取------------------------------------------
    void getPanelSettings();
    void setPanelSettings(TdcQutag::TdcQutagSettings settings);

signals:
    //---------------------------------状态改变信号---------------------------------
    void enableChanged(TdcQutag::TdcQutagSettings settings);
    void signalConditioningChanged(int chIdx,TdcQutag::TdcQutagSettings settings);
    void delayChanged(TdcQutag::TdcQutagSettings settings);
    void deadTimeChanged(int chIdx,TdcQutag::TdcQutagSettings settings);

private:
    Ui::TdcQutagConfig *ui;
    //------------------------------------成员变量------------------------------------
    TdcQutag::TdcQutagSettings m_tdcSettings;

    //------------------------------------状态改变发射信号--------------------------------------
    //warning:值改变就会改一次，而不是一个值写完再改
    void on_enableChanged();
    void on_signalConditionChanged(int chIdx);
    void on_delayChanged();
    void on_deadTimeChanged(int chIdx);
};

#endif // TDCQUTAGCONFIG_H
