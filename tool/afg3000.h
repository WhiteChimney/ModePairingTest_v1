#ifndef AFG3000_H
#define AFG3000_H

#include <QObject>
#include <QString>
#include <visa.h>

#define MAX_REC_SIZE 255

class AFG3000 : public QObject
{
    Q_OBJECT
public:
    explicit AFG3000(QString resourceName, QObject *parent = nullptr);
    ~AFG3000();


private:

    QString resourceName;
//    VISA session
    ViStatus status = VI_SUCCESS - 1;
    ViSession defaultRM = 0;
    ViSession instr;
    ViUInt32 retCount = 0;

//    读取与写入 buffer
    char* SendAddr = NULL;
    char* SendBuf = NULL;
    unsigned char RecBuf[MAX_REC_SIZE];


public:
    bool initializeDevice();
    bool closeDevice();
    bool sendCommand(QString command);
    bool readReply(QString* reply);
    QString readReply();

    double getDelay(int channel);
    bool setDelay(int channel, double voltage);
};


#endif // AFG3000_H
