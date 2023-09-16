#ifndef LNEUNIT_H
#define LNEUNIT_H

#pragma once
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QStyle>
#include <QDebug>
#include <cmath>

class lneUnit : public QLineEdit
{
public:
    lneUnit(QWidget* parent = Q_NULLPTR);

    //获取编辑框单位
    QString UnitText()const;
    double DoubleValue()const;

    //设置编辑框单位
    void SetUnitText(const QString& unitText);

    void on_text_changed();

private:
    QPushButton * m_unit_btn = nullptr; //单位文本按钮
    double m_double_value = 0;
    QString m_string_value = " ";



    QString UnitChangeForPos(double value);
    QString UnitChangeForNeg(double value);
};

#endif // LNEUNIT_H
