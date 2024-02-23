#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->txbFilePath->setText(m_filePath);
    ui->lneStartPeri->SetUnitText("Hz");
    ui->lneSysPeri->SetUnitText("Hz");

    startUpCustomizedFunctions();

    m_timer = new QElapsedTimer;
    m_tdcQutag = new TdcQutag(this);
    m_tdcQutagConfig = new TdcQutagConfig(this);
    m_tdcQutagDataProcess = new TdcQutagDataProcess(this);
    m_typeConversion = new TypeConversion(this);
    m_tekAWG5200 =  new TekAWG5200(this);
    m_auxiliaryFuncition = new AuxiliaryFunction(this);


    //错误信息
    connect(m_tdcQutag,&TdcQutag::sendInfo,this,&MainWindow::revInfo);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendInfo,this,&MainWindow::revInfo);

    //设置频率
    ui->lneStartPeri->on_text_changed();
    ui->lneSysPeri->on_text_changed();
    connect(ui->btnSetStartFre,&QPushButton::clicked,this,&MainWindow::on_btn_set_frequency);
    connect(ui->btnSetSysFre,&QPushButton::clicked,this,&MainWindow::on_btn_set_frequency);
    //tdc连接
    connect(ui->btnTdcConnect,&QPushButton::clicked,this,[=](){m_tdcQutag->TdcQutag::instrInit(-1);});
    //设置文件路径
    connect(ui->btnSetFilePath,&QPushButton::clicked,this,&MainWindow::on_btn_set_filePath);
    connect(this,&MainWindow::sendFilePath,m_tdcQutagDataProcess,&TdcQutagDataProcess::on_set_file_path);
    //设置大延时
    connect(ui->btnSetExtBigDelay,&QPushButton::clicked,this,[=](){emit sendExtBigDelay(ui->cmbChExtBigDelay->currentIndex(),ui->spbValueExtBigDelay->value());});
    connect(this,&MainWindow::sendExtBigDelay,m_tdcQutagDataProcess,&TdcQutagDataProcess::on_set_big_delay);
    //设置小延时
    connect(ui->btnSetExtSmallDelay,&QPushButton::clicked,this,[=](){emit sendExtSmallDelay(ui->cmbChExtSmallDelay->currentIndex(),ui->spbValueExtSmallDelay->value());});
    connect(this,&MainWindow::sendExtSmallDelay,m_tdcQutagDataProcess,&TdcQutagDataProcess::on_set_small_delay);
    //读取时间戳并进行整形
    connect(ui->btnReadTimeStamps,&QPushButton::clicked,m_tdcQutag,&TdcQutag::getTimeStamps);
    connect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,this,&MainWindow::on_read_time_stamps);


    //获取探测器之间的延时差
    connect(ui->btnGetDelayBetSPD, &QPushButton::clicked, this, &MainWindow::on_get_delay_between_spd);

    //扫描SPD1延时
    connect(ui->btnScanSPD1Delay, &QPushButton::clicked, this, [=](){
        int delay = on_scan_spd_delay(1, 1, 1);
        ui->lcdSPD1Delay->display(delay);
    });
    //扫描SPD2延时
    connect(ui->btnScanSPD2Delay, &QPushButton::clicked, this, [=](){
        int delay = on_scan_spd_delay(2, 1, 2);
        ui->lcdSPD2Delay->display(delay);
    });
    //连续运行采集TDC计数
    connect(ui->btnRunTdcConti, &QPushButton::clicked, this, &MainWindow::on_plot_tdc);

    //获取符合计数
    connect(ui->btnCoincNum, &QPushButton::clicked, this, &MainWindow::on_req_coinc_number_of_one_period);

    //设置tdc参数面板
    connect(ui->actionQutagConfig,&QAction::triggered,this,[=](){
        if (m_tdcQutag->GetDeviceType()==1) {m_tdcQutag->TdcQutag::instrInit(-1);}
        TdcQutag::TdcQutagSettings settings;
        m_tdcQutagConfig->show();
        m_tdcQutag->on_getCurrentSettings(& settings);
        m_tdcQutagConfig->setPanelSettings(settings);
    });
    connect(m_tdcQutagConfig,&TdcQutagConfig::enableChanged,m_tdcQutag,&TdcQutag::on_setChannelEnable);
    connect(m_tdcQutagConfig,&TdcQutagConfig::signalConditioningChanged,m_tdcQutag,&TdcQutag::on_setSignalCondition);
    connect(m_tdcQutagConfig,&TdcQutagConfig::delayChanged,m_tdcQutag,&TdcQutag::on_setDelay);
    connect(m_tdcQutagConfig,&TdcQutagConfig::deadTimeChanged,m_tdcQutag,&TdcQutag::on_setDeadTime);

    //设置AWG5200参数面板
    connect(ui->actionTekAWG5200, &QAction::triggered,this, [=](){
        m_tekAWG5200->show();
    });
    //测试
    connect(ui->btnTest,&QPushButton::clicked,this,&MainWindow::on_btn_test);

    LoadIni();


}

MainWindow::~MainWindow()
{
    setIni();
    delete ui;
}

//info显示
void MainWindow::revInfo(bool state, qint32 errorCode, QString info) //true为无错误
{
    QString str = QDateTime::currentDateTime().toString(tr("yyyy-MM-dd hh:mm:ss "));
    if(!state)
    {
        str.append(QString(" Error Code=%1,%2").arg(errorCode).arg(info));
    }
    else str.append(info);
    ui->txeRetInfo->append(str);
}



void MainWindow::on_btn_set_filePath()
{
    m_filePath = QFileDialog::getOpenFileName(this,tr("文件"),"",tr("text(*.txt)"));
    ui->txbFilePath->setText(m_filePath);

    emit sendFilePath(ui->txbFilePath->toPlainText());
}



void MainWindow::on_reshape_time_stamps(Int64 m_timeStampsOfCh[CH_NUM][BUFFER_SIZE], int m_numOfTSOfCh[5])
{
//    qDebug() << m_numOfTSOfCh[0] << m_numOfTSOfCh[1] << m_numOfTSOfCh[2] << m_numOfTSOfCh[3];

//    QString info = "Valid number for each channel is:" + QString::number(m_numOfTSOfCh[0]) + " "
//            + QString::number(m_numOfTSOfCh[1]) + " "+ QString::number(m_numOfTSOfCh[2]) + " "
//            + QString::number(m_numOfTSOfCh[3]) + " "+ QString::number(m_numOfTSOfCh[4]);
//    revInfo(true, 0, info);
}




void MainWindow::on_btn_set_frequency()
{
    m_tdcQutagDataProcess->on_set_period(ui->lneStartPeri->DoubleValue(), ui->lneSysPeri->DoubleValue());
}

void MainWindow::on_btn_test()
{

}


void MainWindow:: on_status_true()
{
    m_status = 1;
}

int MainWindow::on_scan_spd_delay(int tdcCh, int awgCh, int awgMarker)
{
    double start = ui->spbSpdDelayStart->value();
    double stop = ui->spbSpdDelayStop->value();
    double interval = ui->spbSpdDelayInterval->value();
    int time = ui->spbSpdDelayTime->value();
    int round = (stop-start) / interval;

    connect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,&TdcQutagDataProcess::reshapeTimeStamps);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_reshape_time_stamps);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_status_true);

    m_tekAWG5200->VisaOpen();

    m_histogramX.clear();
    m_histogramX.resize(round);
    m_histogramY.clear();
    m_histogramY.resize(round);

    m_timer->start();
    for (int idx = 0; idx < round; idx++)
    {
        int scanVal = idx * interval;//扫描间隔为XXps
        m_histogramX[idx] = scanVal;

        m_tekAWG5200->SetMarkerDelay (awgCh, awgMarker, scanVal);
        while (! m_timer->hasExpired(100))//写入后100ms开始计数
        {
            QCoreApplication::processEvents();
        }
        m_tdcQutag->clearTimeStamps();
        m_timer->restart();

        while (! m_timer->hasExpired(time))//数据积累时间为XXms
        {
            QCoreApplication::processEvents();
        }
        m_tdcQutag->getTimeStamps();
        while (!m_status)
        {
            QCoreApplication::processEvents();
        }
        m_status = 0 ;
        m_histogramY[idx] = m_tdcQutagDataProcess->on_get_number_of_channel(tdcCh);

        SimplePlot(m_histogramX, m_histogramY);

        if(ui->btnStop->on_get_state() == true)
        {
            ui->btnStop->on_clicked();
            break;
        }
    }
    m_timer->elapsed();

    int max;
    int maxPos;
    m_auxiliaryFuncition->SearchMaxAndPosi(m_histogramY, max, maxPos);

    qDebug() << "scan succeful!" << "max value is" << QString::number(max) << ";"
             << "max delay is" << QString::number(maxPos*interval);
    m_tekAWG5200->SetMarkerDelay (awgCh, awgMarker, maxPos*interval);

    m_tekAWG5200->VisaClose();
    disconnect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,&TdcQutagDataProcess::reshapeTimeStamps);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_reshape_time_stamps);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_status_true);

    return maxPos*interval;
}



void MainWindow::on_get_delay_between_spd()
{
    connect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,&TdcQutagDataProcess::reshapeTimeStamps);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_reshape_time_stamps);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_status_true);

    m_tdcQutag->clearTimeStamps();
    m_timer->start();
    while (! m_timer->hasExpired(100))//清空后等待100ms
    {
        QCoreApplication::processEvents();
    }
    m_timer->elapsed();

    m_tdcQutag->getTimeStamps();
    while (!m_status)
    {
        QCoreApplication::processEvents();
    }
    m_status = 0 ;

    m_tdcQutagDataProcess->getDelayOfSPD (1, 2);

    disconnect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,&TdcQutagDataProcess::reshapeTimeStamps);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_reshape_time_stamps);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_status_true);

}





void MainWindow::on_req_coinc_number_of_one_period()
{
    int time = 100;
    int chIdx1 = 1;
    int chIdx2 = 2;
    Int64 ch1Num = 0;
    Int64 ch2Num = 0;
    Int64 coincNumOfOnePeri = 0;

    connect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,[=](Int64 timeStamps[BUFFER_SIZE], Int8 channels[BUFFER_SIZE], Int32 * validNum) mutable {
        on_read_time_stamps(timeStamps, channels, validNum);
        emit reqCoincNumOfOnePeri(chIdx1, chIdx2, timeStamps, channels, * validNum);
    });
    connect(this, &MainWindow::reqCoincNumOfOnePeri, m_tdcQutagDataProcess, &TdcQutagDataProcess::getCoincOfOnePeri);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendCoincOfOnePeri,this,[=](){on_status_true();});


    m_tdcQutag->clearTimeStamps();
    m_timer->start();
    while (! m_timer->hasExpired(time))//清空后等待XXms
    {
        QCoreApplication::processEvents();
    }
    m_timer->elapsed();

    m_tdcQutag->getTimeStamps();
    while (!m_status)
    {
        QCoreApplication::processEvents();
    }
    m_status = 0 ;

    m_tdcQutagDataProcess->on_get_coinc_of_one_peri(&ch1Num, &ch2Num, &coincNumOfOnePeri);

    qDebug() << "ch1Num" << ch1Num << "ch2Num" << ch2Num << "coincNumOfOnePeri" << coincNumOfOnePeri;

    qDebug() << "coinc rate = " << coincNumOfOnePeri*time*1e-3*1e9/(ch1Num*ch2Num);


    disconnect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,0,0);
    disconnect(this, &MainWindow::reqCoincNumOfOnePeri,0,0);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendCoincOfOnePeri,0,0);


}
//-----------------------------------绘图函数---------------------------------------------------------
void MainWindow::SimplePlot (QVector<double> &inputX, QVector<double> &inputY)
{  // 这里输入的数据似乎只能是double类的，注意要绘图的东西都得强制转化为double

    QCustomPlot * customPlot = ui->customPlot;
    //创建画布,设置画布上的点数据
    customPlot->addGraph();
    customPlot->graph(0)->setData(inputX,inputY);

    //设置坐标轴标签
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");

    customPlot->xAxis->setVisible(true);
    customPlot->xAxis->setTickLabels(true);
    customPlot->yAxis->setVisible(true);
    customPlot->yAxis->setTickLabels(true);

    customPlot->xAxis->rescale(true);
    customPlot->yAxis->rescale(true);

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //    customPlot->show();

    customPlot->replot();
}

void MainWindow::SimplePlot (QVector<double> &inputX0, QVector<double> &inputY0,
                             QVector<double> &inputX1, QVector<double> &inputY1)
{  // 这里输入的数据似乎只能是double类的，注意要绘图的东西都得强制转化为double

    QCustomPlot * customPlot = ui->customPlot;

    //创建画布
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::darkBlue));
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::darkGreen));


    //设置画布上的点数据
    customPlot->graph(0)->setData(inputX0,inputY0);
    customPlot->graph(1)->setData(inputX1,inputY1);


    //设置坐标轴标签
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->xAxis->setVisible(true);
    customPlot->xAxis->setTickLabels(true);
    customPlot->yAxis->setVisible(true);
    customPlot->yAxis->setTickLabels(true);

    customPlot->xAxis->rescale(true);
    customPlot->yAxis->rescale(true);

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //    customPlot->show();

    customPlot->replot();
}

void MainWindow::SimplePlot (QVector<double> &inputX0, QVector<double> &inputY0,
                             QVector<double> &inputX1, QVector<double> &inputY1,
                             QVector<double> &inputX2, QVector<double> &inputY2,
                             QVector<double> &inputX3, QVector<double> &inputY3)
{  // 这里输入的数据似乎只能是double类的，注意要绘图的东西都得强制转化为double

    QCustomPlot * customPlot = ui->customPlot;

    //创建画布
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::darkBlue));
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::darkGreen));
    customPlot->addGraph();
    customPlot->graph(2)->setPen(QPen(Qt::darkRed));
    customPlot->addGraph();
    customPlot->graph(3)->setPen(QPen(Qt::darkCyan));


    //设置画布上的点数据
    customPlot->graph(0)->setData(inputX0,inputY0);
    customPlot->graph(1)->setData(inputX1,inputY1);
    customPlot->graph(2)->setData(inputX2,inputY2);
    customPlot->graph(3)->setData(inputX3,inputY3);



    //设置坐标轴标签
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->xAxis->setVisible(true);
    customPlot->xAxis->setTickLabels(true);
    customPlot->yAxis->setVisible(true);
    customPlot->yAxis->setTickLabels(true);

    customPlot->xAxis->rescale(true);
    customPlot->yAxis->rescale(true);

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //    customPlot->show();

    customPlot->replot();
}

//配置文件
void MainWindow::LoadIni()  //loadName字符串是 配置文件里的“节”名称
{
    //判断文件是否存在
    QFileInfo fileInfo(m_iniName);
    if (!fileInfo.isFile())
    {
        QMessageBox::critical(NULL, "错误", "缺少config.ini文件！");
        return;
    }
    //读取配置文件
    QSettings settings(m_iniName, QSettings::IniFormat);
    settings.setIniCodec("utf-8");// 解决乱码
    //使用 settings.value(“节名/键名”)方法访问配置文件的值
    //    QString ver = QString("user/name");
    //    QString val = settings.value(ver).toString();//配置文件读取后，要对其进行类型转换
    //    qDebug()<< val;


}

void MainWindow::setIni()
{
    //如果没有这个配置文件，会自动创建
    QSettings * settings;
    settings = new QSettings(m_iniName, QSettings::IniFormat);
    settings->setIniCodec("utf-8");// 解决乱码

    //使用 settings.setValue(“节名/键名”，“值”)方法 输出
    QString ver = QString("user/name");
    QString val = settings->value(ver).toString();

    if (val.isEmpty()) {  //如果没有用户名就写入
        //写入数据
        settings->setValue("/user/name", "admin");
        settings->setValue("/password/mypass", "123456");
    }
    //写入数据
    settings->setValue("/test/name", "admin");
    settings->setValue("/test/mypass", QString::fromLocal8Bit("密码"));//QString::fromLocal8Bit() 解决乱码
    /*  在user的组别下 继续写东西  */
    settings->beginGroup("user");
    settings->setValue("login",1);
    settings->endGroup(); //关闭分组定位,注意如果需切换分组，必须保证关闭。不然切换无效

    delete settings;
}



void MainWindow::on_plot_tdc()
{
    int time = ui->spbRunTime->value();

    QVector<double> histogramX1, histogramY1;
    histogramX1.clear(); histogramX1.resize(100); histogramY1.clear(); histogramY1.resize(100);
    QVector<double> histogramX2, histogramY2;
    histogramX2.clear(); histogramX2.resize(100); histogramY2.clear(); histogramY2.resize(100);
    QVector<double> histogramX3, histogramY3;
    histogramX3.clear(); histogramX3.resize(100); histogramY3.clear(); histogramY3.resize(100);
    QVector<double> histogramX4, histogramY4;
    histogramX4.clear(); histogramX4.resize(100); histogramY4.clear(); histogramY4.resize(100);

    connect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,&TdcQutagDataProcess::reshapeTimeStamps);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_reshape_time_stamps);
    connect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_status_true);

    m_tdcQutag->clearTimeStamps();
    m_timer->start();
//    int idx = 0;
    while (ui->btnStop->on_get_state() == false)
    {
        while (! m_timer->hasExpired(time)) QCoreApplication::processEvents();
        m_tdcQutag->getTimeStamps();
        m_timer->restart();
        while (!m_status) QCoreApplication::processEvents();
        m_status = 0;

//        qDebug() << "running...";

//        histogramX1[idx] = idx;
//        histogramX2[idx] = idx;
//        histogramX3[idx] = idx;
//        histogramX4[idx] = idx;
//        histogramY1[idx] = m_tdcQutagDataProcess->on_get_number_of_channel(1);
//        histogramY2[idx] = m_tdcQutagDataProcess->on_get_number_of_channel(2);
//        histogramY3[idx] = m_tdcQutagDataProcess->on_get_number_of_channel(3);
//        histogramY4[idx] = m_tdcQutagDataProcess->on_get_number_of_channel(4);
//        idx ++;
//        if (idx == histogramX1.size()-1) histogramX1.resize(histogramX1.size()+100);
//        if (idx == histogramX2.size()-1) histogramX2.resize(histogramX2.size()+100);
//        if (idx == histogramX3.size()-1) histogramX3.resize(histogramX3.size()+100);
//        if (idx == histogramX4.size()-1) histogramX4.resize(histogramX4.size()+100);
//        if (idx == histogramY1.size()-1) histogramY1.resize(histogramY1.size()+100);
//        if (idx == histogramY2.size()-1) histogramY2.resize(histogramY2.size()+100);
//        if (idx == histogramY3.size()-1) histogramY3.resize(histogramY3.size()+100);
//        if (idx == histogramY4.size()-1) histogramY4.resize(histogramY4.size()+100);

//        SimplePlot(histogramX1, histogramY1, histogramX2, histogramY2,
//                   histogramX3, histogramY3, histogramX4, histogramY4);


        if(ui->btnStop->on_get_state() == true)
        {
            ui->btnStop->on_clicked();
            break;
        }

        if (ui->btnPlotClear->on_get_state() == true)
        {
//            idx = 0;
//            histogramX1.clear(); histogramX1.resize(100); histogramY1.clear(); histogramY1.resize(100);
//            histogramX2.clear(); histogramX2.resize(100); histogramY2.clear(); histogramY2.resize(100);
//            histogramX3.clear(); histogramX3.resize(100); histogramY3.clear(); histogramY3.resize(100);
//            histogramX4.clear(); histogramX4.resize(100); histogramY4.clear(); histogramY4.resize(100);
            ui->btnPlotClear->on_clicked();
        }
//        break;
    }

    m_timer->elapsed();

    disconnect(m_tdcQutag,&TdcQutag::GetTimeStampsFinished,m_tdcQutagDataProcess,&TdcQutagDataProcess::reshapeTimeStamps);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_reshape_time_stamps);
    disconnect(m_tdcQutagDataProcess,&TdcQutagDataProcess::sendReTimeStamps,this,&MainWindow::on_status_true);

    wrapUpCustomizedFunctions();

}


