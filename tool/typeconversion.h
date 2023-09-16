#ifndef TYPECONVERSION_H
#define TYPECONVERSION_H

#include <QObject>
#include <QDebug>

class TypeConversion : public QObject
{
    Q_OBJECT
public:
    explicit TypeConversion(QObject *parent = nullptr);

    char * QString2charArr(QString str);
    char * Float2charArr(float ipt, char opt[]);

signals:

};

#endif // TYPECONVERSION_H
