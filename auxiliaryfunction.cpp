#include "auxiliaryfunction.h"

AuxiliaryFunction::AuxiliaryFunction(QObject *parent) : QObject(parent)
{

}

void AuxiliaryFunction::SearchMaxAndPosi(QVector<double> & ipt, int & max, int & max_Posi)
{
    // 功能：寻找将QVector中的最大和最小元素并用min 和 max这两个引用参数进行输出
    // 同时记录下min与max所在的位置并用min_Posi和max_Posi两个参数进行输出
    int len = ipt.length();
    if (len==0)
    {
        max = 0;
        max_Posi = 0;

    }
    else
    {
        max = ipt[0];
        max_Posi = 0;
        for (int idx=0;idx<len;idx++)
        {
            if ( ipt[idx]>max)
            {
                max = ipt[idx];
                max_Posi = idx;
            }
        }
    }

}

void AuxiliaryFunction::WriteFile(QString filePath, QVector<double> arr1, QVector<double> arr2)
{
    QFile outfile(filePath);

    if(outfile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&outfile);
        out.setCodec("UTF-8");

        for (int idx = 0; idx < arr1.size(); idx++)
        {
            out << arr1.at(idx) << "\t" << arr2.at(idx) << "\n";
        }

        outfile.close();
    }
    else
    {
        qDebug() << "open false";
    }
}
