#include "afg3000.h"

AFG3000::AFG3000(QString resourceName0, QObject *parent) : QObject(parent)
{
    resourceName = resourceName0;
}

AFG3000::~AFG3000()
{
}

bool AFG3000::initializeDevice()
{
//    仪器地址
    SendAddr = resourceName.toLocal8Bit().data();

//    打开设备，获取指针
    status = viOpenDefaultRM(&defaultRM);
    if (status < VI_SUCCESS)
        return false;
    status = viOpen(defaultRM,SendAddr,VI_NULL,VI_NULL,&instr);

    if (status == VI_SUCCESS)
        return true;
    else
        return false;
}

bool AFG3000::closeDevice()
{
    if (status == VI_SUCCESS)
    {
        status = viClose(instr);
        status = viClose(defaultRM);

        if (status == VI_SUCCESS)
            return true;
        else
            return false;
    }
    else
        return false;
}

bool AFG3000::sendCommand(QString command)
{
//    发送指令
    if (status == VI_SUCCESS)
    {
        command += "\n";
        SendBuf = command.toLocal8Bit().data();
        status = viWrite(instr,(unsigned char*)SendBuf,strlen(SendBuf),&retCount);

        if (status == VI_SUCCESS)
            return true;
        else
            return false;
    }
    else
        return false;
}

bool AFG3000::readReply(QString* reply)
{
    if (status == VI_SUCCESS)
    {
        memset(RecBuf,0,MAX_REC_SIZE);
        status = viRead(instr,RecBuf,MAX_REC_SIZE,&retCount);
        if (status == VI_SUCCESS)
        {
            *reply = QString(QLatin1String((char*)RecBuf));
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

QString AFG3000::readReply()
{
    if (status == VI_SUCCESS)
    {
        memset(RecBuf,0,MAX_REC_SIZE);
        status = viRead(instr,RecBuf,MAX_REC_SIZE,&retCount);
        if (status == VI_SUCCESS)
        {
            return QString(QLatin1String((char*)RecBuf));
        }
        else
            return QString("");
    }
    else
        return QString("");
}

double AFG3000::getDelay(int channel = 1)
{
    QString command = "SOURce1:PULSe:DELay?";
//    command += QString::number(channel);
//    command += ",VOLT";
//    sendCommand(command);
    return readReply().toDouble();
}

bool AFG3000::setDelay(int channel, double voltage)
{
    QString command = "SOURce1:PULSe:DELay 20ms";
//    command += QString::number(channel);
//    command += ",";
//    command += QString::number(voltage);
    return sendCommand(command);
}
