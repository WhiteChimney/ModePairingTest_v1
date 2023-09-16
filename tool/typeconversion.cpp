#include "typeconversion.h"

TypeConversion::TypeConversion(QObject *parent) : QObject(parent)
{

}


char * TypeConversion::QString2charArr(QString str)
{
    QByteArray bta = str.toLatin1();
    char * chr = bta.data();
    return chr;
}



char * TypeConversion::Float2charArr(float ipt, char opt[])
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


