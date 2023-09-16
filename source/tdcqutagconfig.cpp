#include "tdcqutagconfig.h"
#include "ui_tdcqutagconfig.h"

TdcQutagConfig::TdcQutagConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TdcQutagConfig)
{
    ui->setupUi(this);

    ui->dsbCH0Threshold->setRange(-2,3);
    ui->dsbCH1Threshold->setRange(-2,3);
    ui->dsbCH2Threshold->setRange(-2,3);
    ui->dsbCH3Threshold->setRange(-2,3);
    ui->dsbCH4Threshold->setRange(-2,3);

    ui->spbCH0DeadTime->setRange(0,100000);
    ui->spbCH1DeadTime->setRange(0,100000);
    ui->spbCH2DeadTime->setRange(0,100000);
    ui->spbCH3DeadTime->setRange(0,100000);
    ui->spbCH4DeadTime->setRange(0,100000);

    ui->spbCH0Delay->setRange(-100000,100000);
    ui->spbCH1Delay->setRange(-100000,100000);
    ui->spbCH2Delay->setRange(-100000,100000);
    ui->spbCH3Delay->setRange(-100000,100000);
    ui->spbCH4Delay->setRange(-100000,100000);

    connect(ui->ckbCH0,&QCheckBox::stateChanged,this,&TdcQutagConfig::on_enableChanged);
    connect(ui->ckbCH1,&QCheckBox::stateChanged,this,&TdcQutagConfig::on_enableChanged);
    connect(ui->ckbCH2,&QCheckBox::stateChanged,this,&TdcQutagConfig::on_enableChanged);
    connect(ui->ckbCH3,&QCheckBox::stateChanged,this,&TdcQutagConfig::on_enableChanged);
    connect(ui->ckbCH4,&QCheckBox::stateChanged,this,&TdcQutagConfig::on_enableChanged);

    connect(ui->cmbCH0SignType,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(0);});
    connect(ui->cmbCH0Trigger,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(0);});
    connect(ui->dsbCH0Threshold,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,[=](){on_signalConditionChanged(0);});
    connect(ui->cmbCH1SignType,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(1);});
    connect(ui->cmbCH1Trigger,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(1);});
    connect(ui->dsbCH1Threshold,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,[=](){on_signalConditionChanged(1);});
    connect(ui->cmbCH2SignType,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(2);});
    connect(ui->cmbCH2Trigger,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(2);});
    connect(ui->dsbCH2Threshold,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,[=](){on_signalConditionChanged(2);});
    connect(ui->cmbCH3SignType,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(3);});
    connect(ui->cmbCH3Trigger,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(3);});
    connect(ui->dsbCH3Threshold,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,[=](){on_signalConditionChanged(3);});
    connect(ui->cmbCH4SignType,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(4);});
    connect(ui->cmbCH4Trigger,qOverload<int>(&QComboBox::currentIndexChanged),
            this,[=](){on_signalConditionChanged(4);});
    connect(ui->dsbCH4Threshold,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,[=](){on_signalConditionChanged(4);});

    connect(ui->spbCH0Delay,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_delayChanged);
    connect(ui->spbCH1Delay,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_delayChanged);
    connect(ui->spbCH2Delay,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_delayChanged);
    connect(ui->spbCH3Delay,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_delayChanged);
    connect(ui->spbCH4Delay,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_delayChanged);

    connect(ui->spbCH0DeadTime,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_deadTimeChanged);
    connect(ui->spbCH1DeadTime,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_deadTimeChanged);
    connect(ui->spbCH2DeadTime,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_deadTimeChanged);
    connect(ui->spbCH3DeadTime,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_deadTimeChanged);
    connect(ui->spbCH4DeadTime,qOverload<int>(&QSpinBox::valueChanged),this,&TdcQutagConfig::on_deadTimeChanged);
}

TdcQutagConfig::~TdcQutagConfig()
{
    delete ui;
}

//读取面板数据至成员变量
void TdcQutagConfig::getPanelSettings()
{
    m_tdcSettings.chEnable[0] = ui->ckbCH0->isChecked();
    m_tdcSettings.chEnable[1] = ui->ckbCH1->isChecked();
    m_tdcSettings.chEnable[2] = ui->ckbCH2->isChecked();
    m_tdcSettings.chEnable[3] = ui->ckbCH3->isChecked();
    m_tdcSettings.chEnable[4] = ui->ckbCH4->isChecked();

    m_tdcSettings.chConditioning[0] = (TDC_SignalCond) (ui->cmbCH0SignType->currentIndex() + 1);
    m_tdcSettings.chConditioning[1] = (TDC_SignalCond) (ui->cmbCH0SignType->currentIndex() + 1);
    m_tdcSettings.chConditioning[2] = (TDC_SignalCond) (ui->cmbCH0SignType->currentIndex() + 1);
    m_tdcSettings.chConditioning[3] = (TDC_SignalCond) (ui->cmbCH0SignType->currentIndex() + 1);
    m_tdcSettings.chConditioning[4] = (TDC_SignalCond) (ui->cmbCH0SignType->currentIndex() + 1);

    m_tdcSettings.chEdge[0] = ui->cmbCH0Trigger->currentIndex();
    m_tdcSettings.chEdge[1] = ui->cmbCH0SignType->currentIndex();
    m_tdcSettings.chEdge[2] = ui->cmbCH0SignType->currentIndex();
    m_tdcSettings.chEdge[3] = ui->cmbCH0SignType->currentIndex();
    m_tdcSettings.chEdge[4] = ui->cmbCH0SignType->currentIndex();

    m_tdcSettings.chThreshold[0] = ui->dsbCH0Threshold->value();
    m_tdcSettings.chThreshold[1] = ui->dsbCH1Threshold->value();
    m_tdcSettings.chThreshold[2] = ui->dsbCH2Threshold->value();
    m_tdcSettings.chThreshold[3] = ui->dsbCH3Threshold->value();
    m_tdcSettings.chThreshold[4] = ui->dsbCH4Threshold->value();

    m_tdcSettings.chDelay[0] = ui->spbCH0Delay->value();
    m_tdcSettings.chDelay[1] = ui->spbCH1Delay->value();
    m_tdcSettings.chDelay[2] = ui->spbCH2Delay->value();
    m_tdcSettings.chDelay[3] = ui->spbCH3Delay->value();
    m_tdcSettings.chDelay[4] = ui->spbCH4Delay->value();

    m_tdcSettings.chDeadTime[0] = ui->spbCH0DeadTime->value();
    m_tdcSettings.chDeadTime[1] = ui->spbCH0DeadTime->value();
    m_tdcSettings.chDeadTime[2] = ui->spbCH0DeadTime->value();
    m_tdcSettings.chDeadTime[3] = ui->spbCH0DeadTime->value();
    m_tdcSettings.chDeadTime[4] = ui->spbCH0DeadTime->value();

}

//设置面板数据
void TdcQutagConfig::setPanelSettings(TdcQutag::TdcQutagSettings settings)
{
    ui->ckbCH0->setCheckState((settings.chEnable[0]? Qt::Checked : Qt::Unchecked));
    ui->ckbCH1->setCheckState((settings.chEnable[1]? Qt::Checked : Qt::Unchecked));
    ui->ckbCH2->setCheckState((settings.chEnable[2]? Qt::Checked : Qt::Unchecked));
    ui->ckbCH3->setCheckState((settings.chEnable[3]? Qt::Checked : Qt::Unchecked));
    ui->ckbCH4->setCheckState((settings.chEnable[4]? Qt::Checked : Qt::Unchecked));

    ui->cmbCH0SignType->setCurrentIndex((int) settings.chConditioning[0] - 1);
    ui->cmbCH1SignType->setCurrentIndex((int) settings.chConditioning[1] - 1);
    ui->cmbCH2SignType->setCurrentIndex((int) settings.chConditioning[2] - 1);
    ui->cmbCH3SignType->setCurrentIndex((int) settings.chConditioning[3] - 1);
    ui->cmbCH4SignType->setCurrentIndex((int) settings.chConditioning[4] - 1);

    ui->cmbCH0Trigger->setCurrentIndex(settings.chEdge[0]);
    ui->cmbCH1Trigger->setCurrentIndex(settings.chEdge[1]);
    ui->cmbCH2Trigger->setCurrentIndex(settings.chEdge[2]);
    ui->cmbCH3Trigger->setCurrentIndex(settings.chEdge[3]);
    ui->cmbCH4Trigger->setCurrentIndex(settings.chEdge[4]);

    ui->dsbCH0Threshold->setValue(settings.chThreshold[0]);
    ui->dsbCH1Threshold->setValue(settings.chThreshold[1]);
    ui->dsbCH2Threshold->setValue(settings.chThreshold[2]);
    ui->dsbCH3Threshold->setValue(settings.chThreshold[3]);
    ui->dsbCH4Threshold->setValue(settings.chThreshold[4]);

    ui->spbCH0Delay->setValue(settings.chDelay[0]);
    ui->spbCH1Delay->setValue(settings.chDelay[1]);
    ui->spbCH2Delay->setValue(settings.chDelay[2]);
    ui->spbCH3Delay->setValue(settings.chDelay[3]);
    ui->spbCH4Delay->setValue(settings.chDelay[4]);

    ui->spbCH0DeadTime->setValue(settings.chDeadTime[0]);
    ui->spbCH1DeadTime->setValue(settings.chDeadTime[1]);
    ui->spbCH2DeadTime->setValue(settings.chDeadTime[2]);
    ui->spbCH3DeadTime->setValue(settings.chDeadTime[3]);
    ui->spbCH4DeadTime->setValue(settings.chDeadTime[4]);

    getPanelSettings();
}

//-------------------------------------------面板状态改变函数-------------------------------------------
void TdcQutagConfig::on_enableChanged()
{
    getPanelSettings();
    emit enableChanged(m_tdcSettings);
}

void TdcQutagConfig::on_signalConditionChanged(int chIdx)
{
    if (chIdx == 0 && ui->cmbCH0SignType->currentIndex() == 0)
    {
        ui->cmbCH0Trigger->setCurrentIndex(0); ui->cmbCH0Trigger->setEnabled(false);
        ui->dsbCH0Threshold->setValue(2); ui->dsbCH0Threshold->setEnabled(false);
    }
    if (chIdx == 0 && ui->cmbCH0SignType->currentIndex() == 1)
    {
        ui->cmbCH0Trigger->setCurrentIndex(0); ui->cmbCH0Trigger->setEnabled(false);
        ui->dsbCH0Threshold->setValue(0.6); ui->dsbCH0Threshold->setEnabled(false);
    }
    if (chIdx == 0 && ui->cmbCH0SignType->currentIndex() == 2)
    {
        ui->cmbCH0Trigger->setEnabled(true);
        ui->dsbCH0Threshold->setEnabled(true);
    }

    if (chIdx == 1 && ui->cmbCH1SignType->currentIndex() == 0)
    {
        ui->cmbCH1Trigger->setCurrentIndex(0); ui->cmbCH1Trigger->setEnabled(false);
        ui->dsbCH1Threshold->setValue(2); ui->dsbCH1Threshold->setEnabled(false);
    }
    if (chIdx == 1 && ui->cmbCH1SignType->currentIndex() == 1)
    {
        ui->cmbCH1Trigger->setCurrentIndex(0); ui->cmbCH1Trigger->setEnabled(false);
        ui->dsbCH1Threshold->setValue(0.6); ui->dsbCH1Threshold->setEnabled(false);
    }
    if (chIdx == 1 && ui->cmbCH1SignType->currentIndex() == 2)
    {
        ui->cmbCH1Trigger->setEnabled(true);
        ui->dsbCH1Threshold->setEnabled(true);
    }

    if (chIdx == 2 && ui->cmbCH2SignType->currentIndex() == 0)
    {
        ui->cmbCH2Trigger->setCurrentIndex(0); ui->cmbCH2Trigger->setEnabled(false);
        ui->dsbCH2Threshold->setValue(2); ui->dsbCH2Threshold->setEnabled(false);
    }
    if (chIdx == 2 && ui->cmbCH2SignType->currentIndex() == 1)
    {
        ui->cmbCH2Trigger->setCurrentIndex(0); ui->cmbCH2Trigger->setEnabled(false);
        ui->dsbCH2Threshold->setValue(0.6); ui->dsbCH2Threshold->setEnabled(false);
    }
    if (chIdx == 2 && ui->cmbCH2SignType->currentIndex() == 2)
    {
        ui->cmbCH2Trigger->setEnabled(true);
        ui->dsbCH2Threshold->setEnabled(true);
    }

    if (chIdx == 3 && ui->cmbCH3SignType->currentIndex() == 0)
    {
        ui->cmbCH3Trigger->setCurrentIndex(0); ui->cmbCH3Trigger->setEnabled(false);
        ui->dsbCH3Threshold->setValue(2); ui->dsbCH3Threshold->setEnabled(false);
    }
    if (chIdx == 3 && ui->cmbCH3SignType->currentIndex() == 1)
    {
        ui->cmbCH3Trigger->setCurrentIndex(0); ui->cmbCH3Trigger->setEnabled(false);
        ui->dsbCH3Threshold->setValue(0.6); ui->dsbCH3Threshold->setEnabled(false);
    }
    if (chIdx == 3 && ui->cmbCH3SignType->currentIndex() == 2)
    {
        ui->cmbCH3Trigger->setEnabled(true);
        ui->dsbCH3Threshold->setEnabled(true);
    }

    if (chIdx == 4 && ui->cmbCH4SignType->currentIndex() == 0)
    {
        ui->cmbCH4Trigger->setCurrentIndex(0); ui->cmbCH4Trigger->setEnabled(false);
        ui->dsbCH4Threshold->setValue(2); ui->dsbCH4Threshold->setEnabled(false);
    }
    if (chIdx == 4 && ui->cmbCH4SignType->currentIndex() == 1)
    {
        ui->cmbCH4Trigger->setCurrentIndex(0); ui->cmbCH4Trigger->setEnabled(false);
        ui->dsbCH4Threshold->setValue(0.6); ui->dsbCH4Threshold->setEnabled(false);
    }
    if (chIdx == 4 && ui->cmbCH4SignType->currentIndex() == 2)
    {
        ui->cmbCH4Trigger->setEnabled(true);
        ui->dsbCH4Threshold->setEnabled(true);
    }

    getPanelSettings();
    emit signalConditioningChanged(chIdx,m_tdcSettings);
}

void TdcQutagConfig::on_delayChanged()
{
    getPanelSettings();
    emit delayChanged(m_tdcSettings);
}

void TdcQutagConfig::on_deadTimeChanged(int chIdx)
{
    getPanelSettings();
    emit deadTimeChanged(chIdx,m_tdcSettings);
}
