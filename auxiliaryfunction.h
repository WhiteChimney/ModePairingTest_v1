#ifndef AUXILIARYFUNCTION_H
#define AUXILIARYFUNCTION_H

#include <QObject>
#include <QtDebug>
#include <QFile>

class AuxiliaryFunction : public QObject
{
    Q_OBJECT
public:
    explicit AuxiliaryFunction(QObject *parent = nullptr);

    void WriteFile(QString filePath, QVector<double> arr1, QVector<double> arr2);
    void SearchMaxAndPosi(QVector<double> & ipt, int & max, int & max_Posi);

signals:

};

#endif // AUXILIARYFUNCTION_H
