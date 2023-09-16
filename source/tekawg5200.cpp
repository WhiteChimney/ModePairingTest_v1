#include "tekawg5200.h"
#include "ui_tekawg5200.h"

TekAWG5200::TekAWG5200(QWidget *parent)
    : QDialog{parent},
    ui(new Ui::TekAWG5200)
{
    ui->setupUi(this);

    connect(ui->btnOpenVisa,&QPushButton::clicked,this,&TekAWG5200::on_btn_open_visa);
    connect(ui->btnCloseVisa,&QPushButton::clicked,this,&TekAWG5200::on_btn_close_visa);
    connect(ui->btnQuery,&QPushButton::clicked,this,&TekAWG5200::on_btn_query);
    connect(ui->btnOpenCh,&QPushButton::clicked,this,&TekAWG5200::on_btn_open_channel);
    connect(ui->btnCloseCh,&QPushButton::clicked,this,&TekAWG5200::on_btn_close_channel);
    connect(ui->btnTest,&QPushButton::clicked,this,&TekAWG5200::on_btn_test);

}



int TekAWG5200::VisaOpen()
{
    // Returns a session to the Default Resource Manager.
    m_status = viOpenDefaultRM(& m_defaultRM);
    if (m_status < VI_SUCCESS) {
        qDebug() << "error in viOpenDefaultRM";
        return 1;
    }

    //Opens a session to the specified resource
    char * opt = QString2charArr(m_visaAddr);
    m_status = viOpen(m_defaultRM, static_cast<ViRsrc>(opt), VI_NULL, VI_NULL, & m_instr);
    if (m_status < VI_SUCCESS) {
        qDebug() << "error in viOpen";
        return 2;
    }

    qDebug() << "visa is opened.";
    return 0;
}

int TekAWG5200::VisaClose()
{
    m_status = viClose(m_instr);
    if (m_status < VI_SUCCESS) {
        qDebug() << "error in m_instr Close";
        return 1;
    }

    m_status = viClose(m_defaultRM);
    if (m_status < VI_SUCCESS) {
        qDebug() << "error in m_defaultRM Close";
        return 2;
    }

    qDebug() << "visa is closed.";
    return 0;
}

int TekAWG5200::QueryIDN()
{
    QString readBuf;
    m_status = InstrQueryf("*IDN?\n", "%s", & readBuf);
    qDebug() << readBuf;
    return m_status;
}

ViStatus TekAWG5200::InstrWrite (QString cmdStr)
{
    ViChar statusBuf[256];
    char * cmdChr = QString2charArr(cmdStr);
    int strLength = cmdStr.length();

    m_status = viWrite(m_instr, reinterpret_cast<ViBuf>(cmdChr), strLength, & m_retCnt);
    if (m_status == 0) {
        qDebug() << cmdStr << "write successful!";
    }
    else {
        qDebug() << cmdStr << "write false!";
        viStatusDesc(m_instr, m_status, statusBuf);
        qDebug() << statusBuf;
    }

    return m_status;
}

ViStatus TekAWG5200::InstrRead(QString cmdStr, QString * retStr)
{
    ViChar statusBuf[256];
    unsigned char recBuf[100];
    char * cmdChr = QString2charArr(cmdStr);
    int strLength = cmdStr.length();

    m_status = viWrite(m_instr, reinterpret_cast<ViBuf>(cmdChr), strLength, & m_retCnt);

    if (m_status == 0) {
        m_status = viRead(m_instr, recBuf, m_maxCnt, & m_retCnt);
        if (m_status == 0) {
            * retStr = QString::fromLocal8Bit((char *) recBuf , m_retCnt);
        }
        else {
            qDebug() << cmdStr << "read false!";
            viStatusDesc(m_instr, m_status, statusBuf);
            qDebug() << statusBuf;
        }
    }
    else {
        qDebug() << cmdStr << "write false!";
        viStatusDesc(m_instr, m_status, statusBuf);
        qDebug() << statusBuf;
    }

    qDebug() << cmdStr << "read successful!";
    return m_status;
}

ViStatus TekAWG5200::InstrQueryf(QString cmdStr, QString readFmtStr, QString * retStr)
{
    ViChar readBuf[256], statusBuf[256];
    char * cmd = QString2charArr(cmdStr);
    char * readFmt = QString2charArr(readFmtStr);

    m_status = viQueryf(m_instr, (ViString) cmd , (ViString) readFmt, readBuf);

    if (m_status == 0) {
        * retStr = QString::fromUtf8(readBuf);
    }
    else {
        viStatusDesc(m_instr, m_status, statusBuf);
        qDebug() << statusBuf;
        qDebug() << cmdStr << "query false!";
    }

    qDebug() << cmdStr << "query successful!";
    return m_status;
}

int TekAWG5200::FindSources() // 查找所有连接的资源(ASRL1的设备没有描述)
{
    m_status = viOpenDefaultRM(& m_defaultRM);//打开默认的session
    if (m_status < VI_SUCCESS) goto error;

    ViFindList list;
    ViUInt32 itemCnt;
    ViChar desc[256], id[256], buffer[256];
    m_status = viFindRsrc(m_defaultRM, "USB0?*INSTR", & list, & itemCnt, desc);//查找资源
    if (m_status < VI_SUCCESS) goto error;

    for (int idx = 0; idx < itemCnt; idx ++) {
        qDebug() << "The location of the device is:" << desc;

        m_status = viOpen(m_defaultRM, desc, VI_NULL, VI_NULL, & m_instr);
        if (m_status < VI_SUCCESS) goto error;

        m_status = viWrite(m_instr, (ViBuf) "*idn?", 5, & m_retCnt);
        if (m_status < VI_SUCCESS) goto error;

        memset(id, 0, sizeof(id));
        m_status = viRead(m_instr, (ViBuf) id, sizeof(id), &m_retCnt);
        qDebug() << "The Information of the device is:" << id;

        viClose(m_instr);
        viFindNext(list, desc);
    }

    viClose(m_defaultRM);
    return 0;

error:
    viStatusDesc(m_instr, m_status, buffer);
    qDebug() << buffer;
    if (m_defaultRM != VI_NULL) viClose(m_defaultRM);
    return 1;
}


int TekAWG5200::OpenChannel(int chIdx) // 开启指定通道
{
    QString cmdStr = "OUTPUT" + QString::number(chIdx) + ":STATE ON";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}

int TekAWG5200::CloseChannel(int chIdx) // 开启指定通道
{
    QString cmdStr = "OUTPUT" + QString::number(chIdx) + ":STATE OFF";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}


int TekAWG5200::ChEnabled (int chIdx)
{
    QString retStr;
    QString cmdStr = "OUTPUT" + QString::number(chIdx) + ":STATE?";
    m_status = InstrRead(cmdStr, & retStr);
    if (m_status < VI_SUCCESS) return 1;

    if (retStr[0] == "0") {
        qDebug() << "CH" << chIdx << "is unabled.";
    }
    else if (retStr[0] == "1") {
        qDebug() << "CH" << chIdx << "is enabled.";
    }

    return 0;
}

int TekAWG5200::CreatNewWavelist (QString fileName, int pointNum )
{
    QString cmdStr = ":wlist:waveform:new \"" + fileName + "\"," + QString::number(pointNum) + ",real;";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}

int TekAWG5200::ComplieBasicWaveForm (QString fileName, QString function, QString autoCalc,
                                     QString frequency, QString length, QString samRate, QString cycle,
                                     QString amplitude, QString offset)
{
    char buffer[256];

    QString cmdStr = "BWAVeform:COMPile:NAME \"" + fileName + "\"";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVeform:FUNCtion \"" + function + "\"";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVeform:AUTO" + autoCalc;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:FREQUENCY" + frequency;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:LENGth" + length;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:SRATe" + samRate;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:CYCLe" + cycle;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVeform:AUTO" + autoCalc;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:AMPLitude" + amplitude;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:OFFSet" + offset;
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    cmdStr = "BWAVEFORM:COMPILE";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return m_status;

}

int TekAWG5200::GetList ()
{
    QString retStr;
    QString cmdStr = "WLIST:LIST?";

    m_status = InstrQueryf(cmdStr, "%s", & retStr);
    if (m_status < VI_SUCCESS) return 1;

    retStr = retStr.remove(QChar('\"'));
    QStringList list = retStr.split(",");//QString字符串分割函数
    qDebug() << "波形文件名:";
    foreach (QString variable, list) {
        qDebug() << variable;
    }

    return 0;

}

int TekAWG5200::SetWaveformToCh(int chIdx, QString fileName)
{
    QString cmdStr;
    cmdStr = "SOURCE" + QString::number(chIdx) + ":CASSET:WAVEFORM \"" + fileName + "\"";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}

int TekAWG5200::SetSKEW (int chIdx, int delay)
{
    QString cmdStr;
    cmdStr = "SOURCE" + QString::number(chIdx) + ":SKEW " + QString::number(delay) + "PS; ";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}

int TekAWG5200::GetSKEW (int chIdx)
{
    QString cmdStr, retStr;
    cmdStr = "SOURCE" + QString::number(chIdx) + ":SKEW?";
    m_status = InstrRead(cmdStr, & retStr);
    if (m_status < VI_SUCCESS) return 1;
    qDebug() << "CH" << chIdx << "delay:" << retStr;

    return 0;
}

int TekAWG5200::SetMarkerDelay (int chIdx, int markerIdx, int delay)
{
    QString cmdStr = "SOURCE" + QString::number(chIdx) + ":MARKER" + QString::number(markerIdx)
                     + ":DELAY " + QString::number(delay) + "PS";
    m_status = InstrWrite(cmdStr);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}

int TekAWG5200::GetMarkerDelay (int chIdx, int markerIdx)
{
    QString cmdStr, retStr;
    cmdStr = "SOURCE" + QString::number(chIdx) + ":MARKER" + QString::number(markerIdx)
             + ":DELAY?";
    m_status = InstrRead(cmdStr, & retStr);
    if (m_status < VI_SUCCESS) return 1;
    qDebug() << "CH" << chIdx << "marker" << markerIdx << "delay:" << retStr;

    return 0;
}

int TekAWG5200::WriteWlistAnaData(QString & fileName, QVector<float> & anaData, int beginPos = 0 )
{
    // beginPosi是值要写入波形的初始位置（最好设置成0，别乱搞）

    unsigned char charData[210000] = {0}; // 把float数据转化为char型的数据后存在这里
    unsigned char anaCmd[210100] = {0}; // 最终的命令导入到这里

    // 开始构造要写入的命令
    QString cmdStr = ":wlist:waveform:data \"" + fileName + "\"," + QString::number(beginPos) + ",#";

    int dataSize = anaData.length();
    int headLen = cmdStr.number(dataSize*4).length();
    cmdStr.append(cmdStr.number(headLen));
    cmdStr.append(cmdStr.number(dataSize*4));

    int headLen1=0;
    char * tmpChr = QString2charArr(cmdStr);
    qDebug() << "cmdStr" << cmdStr;

    for (int idx = 0; idx < strlen(tmpChr) ; idx++)
    {
        anaCmd[idx] = tmpChr[idx];
    }

    // 将float数据转换为char并导入
    char tmpChr2[4] = {0};
    for (int idx = 0; idx < dataSize; idx++) // 转换
    {
        Float2charArr(anaData[idx],tmpChr2);
        charData[idx*4] = tmpChr2[0];
        charData[idx*4+1] = tmpChr2[1];
        charData[idx*4+2] = tmpChr2[2];
        charData[idx*4+3] = tmpChr2[3];
    }

    for (int idx = 0; idx < dataSize*4; idx++) //导入
    {
        anaCmd[headLen1+idx] = charData[idx];
    }

    anaCmd[headLen1 + dataSize*4] = ';';

    m_status = viWrite(m_instr, (ViBuf) anaCmd, headLen1 + dataSize*4 + 1, & m_retCnt);
    if (m_status < VI_SUCCESS) return 1;

    return 0;
}

int TekAWG5200::WriteMarkerData(QString &fileName,
                                QVector<bool> & M1, QVector<bool> & M2,
                                QVector<bool> & M3, QVector<bool> & M4) // 写入marker信号的波形
{
    // fileName是波形的名称
    // M1~M4是四个marker的数据，存放在QVector中
    int dataSize = M1.length();
    char cmdChr[60100] = {0}; // 最终的命令导入到这里

    // 构造命令的header部分
    QString cmdStr = ":wlist:waveform:marker:data \"" + fileName + "\"," + QString::number(0) + ",#";
    int headLen = cmdStr.number(dataSize).length();
    cmdStr.append( cmdStr.number(headLen) );
    cmdStr.append( cmdStr.number(dataSize) );

    //    int tmpL = 0;
    //    char tmpChr[100]={0};
    char * tmpChr = QString2charArr(cmdStr);
    int tmpLen = strlen(tmpChr);

    for (int idx = 0; idx < tmpLen; idx++)
    {
        cmdChr[idx] = tmpChr[idx];
    }

    for (int idx = 0; idx < dataSize ; idx++ )
    {
        cmdChr[tmpLen+idx] = (char) Bools2char(M1[idx],M2[idx],M3[idx],M4[idx]);
    }

    cmdChr[tmpLen+dataSize] = ';';
    //    qDebug()<<"cmdChr"<<cmdChr;
    m_status = viWrite(m_instr, (ViBuf)cmdChr, tmpLen+dataSize+1, & m_retCnt);
    if (m_status < VI_SUCCESS) return 1;

    return 0;

}


char * TekAWG5200::QString2charArr(QString str)
{
    QByteArray bta = str.toLatin1();
    char * chr = bta.data();
    return chr;
}

char * TekAWG5200::Float2charArr(float ipt, char opt[])
{
    typedef union
    {
        float fdata;
        char farr[4];
    } FloatLongType;

    FloatLongType mydata;
    mydata.fdata = ipt;

    opt[0] = mydata.farr[0];
    opt[1] = mydata.farr[1];
    opt[2] = mydata.farr[2];
    opt[3] = mydata.farr[3];

    //    *(float*)opt = ipt;

    qDebug() << * (float * ) opt;

    return opt;

}


char TekAWG5200::Bools2char(bool M1, bool M2, bool M3, bool M4)
{
    bool bitArr[8] = {0,0,0,0,0,0,0,0};
    bitArr[7] = M1; bitArr[6] = M2; bitArr[5] = M3; bitArr[4] = M4;
    char chr = bitArr[0]*1 + bitArr[1]*2 + bitArr[2]*4 + bitArr[3]*8
               + bitArr[4]*16 + bitArr[5]*32 + bitArr[6]*64 + bitArr[7]*128;

    return chr;
}



void TekAWG5200::on_set_visa_addr(QString str)
{
    m_visaAddr = str;
}

void TekAWG5200::on_btn_open_visa()
{
    on_set_visa_addr(ui->lneVisaAddr->text());
    VisaOpen();
}
void TekAWG5200::on_btn_close_visa()
{
    VisaClose();
}
void TekAWG5200::on_btn_query()
{
    QueryIDN();
}
void TekAWG5200::on_btn_open_channel()
{
    OpenChannel(ui->cmbChannel->currentIndex()+1);
}
void TekAWG5200::on_btn_close_channel()
{
    CloseChannel(ui->cmbChannel->currentIndex()+1);
}
void TekAWG5200::on_btn_test()
{

}

// WLISt:WAVeform:SRATe <wfm_name>,<sample_rate> 设置波形列表中指定波形的采样率
// WLISt:WAVeform:SRATe? <wfm_name>  返回波形列表中指定波形的采样率
// WLISt:WAVeform:OFFSet <wfm_name>,<offset>  设置波形列表中指定波形的建议偏移量
// WLISt:WAVeform:OFFSet? <wfm_name> 返回波形列表中指定波形的建议偏移量

// 设置或返回指定通道的指定marker的电压幅度、高电压、低电压、偏置
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate][:AMPLitude] <NRf>
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate][:AMPLitude]?
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate]:HIGH <NRf>
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate]:HIGH?
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate]:LOW <NRf>
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate]:LOW?
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate]:OFFSet<NR3>
// [SOURce[n]:]MARKer[m]:VOLTage[:LEVel][:IMMediate]:OFFSet?


