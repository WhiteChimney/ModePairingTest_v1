#include "lneunit.h"

lneUnit::lneUnit(QWidget* parent): QLineEdit(parent)
{
    //设置右侧对齐以及风格
    setAlignment(Qt::AlignRight);
    setStyleSheet("border-style:solid;color: rgba(216, 216, 216, 1);background-color: rgb(30, 40, 50);border-radius:4px");

    m_unit_btn = new QPushButton;
    //单位按钮扁平无边框
    m_unit_btn->setStyleSheet("border-style:solid;color: rgba(77, 155, 213, 1);");
    m_unit_btn->setFlat(true);
    //单位加入布局
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addStretch();
    layout->setSpacing(0);
    //单位按钮的Margins
    layout->setContentsMargins(0, 0, 13, 0);
    layout->addWidget(m_unit_btn);
    this->setLayout(layout);

    connect(this,&QLineEdit::editingFinished,this,&lneUnit::on_text_changed);
}

QString lneUnit::UnitText() const
{
    return this->m_unit_btn->text();
}

double lneUnit::DoubleValue() const
{
    return m_double_value;
}

void lneUnit::SetUnitText(const QString& unitText)
{
    if (unitText.isEmpty() || this->m_unit_btn->text() == unitText)
    {
        return;
    }

    m_unit_btn->setFont(this->font());
    this->m_unit_btn->setText(unitText);
    // this->m_unitButton->setVisible(true);
    int width = this->fontMetrics().width(m_unit_btn->text());
    m_unit_btn->setFixedWidth(width);
    //到单位开头了就往下显示，防止往后输入的内容有一部分在按钮下面
    QBoxLayout* layout = (QBoxLayout*)this->layout();
    QMargins margins = layout->contentsMargins();
    setTextMargins(margins.left(), margins.top(), width + margins.right(), margins.bottom());
}

void lneUnit::on_text_changed()
{

    QString str = QLineEdit::text();
    if (str[0] == "-") {m_double_value = 0; m_string_value ="0"; QLineEdit::setText("0"); return;}

    int idx = 0;
    QString tmp;
    while ((str[idx] >= '0' && str[idx] <= '9') || str[idx] == ".")
    {
        tmp[idx] = str[idx];
        idx ++;
    }

    if ( UnitText() == "Hz")
    {
        if ((str[str.length()-1] == "G") || (str[str.length()-1] == "g")) m_double_value = tmp.toDouble() * pow(10,9);
        else if ((str[str.length()-1] == "M") || (str[str.length()-1] == "m")) m_double_value = tmp.toDouble() * pow(10,6);
        else if ((str[str.length()-1] == "K") || (str[str.length()-1] == "k")) m_double_value = tmp.toDouble() * pow(10,3);
        else m_double_value = tmp.toDouble();

        m_string_value = UnitChangeForPos(m_double_value);
        QLineEdit::setText(m_string_value);

    }

    if ( UnitText() == "V")
    {
        if (str[str.length()-1] == "N" || str[str.length()-1] == "n") m_double_value = tmp.toDouble() * pow(10,-9);
        else if (str[str.length()-1] == "U" || str[str.length()-1] == "u") m_double_value =  tmp.toDouble() * pow(10,-6);
        else if (str[str.length()-1] == "M" || str[str.length()-1] == "m") m_double_value =  tmp.toDouble() * pow(10,-3);
        else m_double_value = tmp.toDouble();

        m_string_value = UnitChangeForNeg(m_double_value);
        QLineEdit::setText(m_string_value);
    }



}


QString lneUnit::UnitChangeForPos(double value)
{
    QString str;

    if (value < 1000)
    {
        str = QString::number(value);
    }
    else if (value < 1000000)
    {
        str = QString::number(value/1000) + " " + "K";
    }
    else if (value < 1000000000)
    {
        str = QString::number(value/1000000) + " " + "M";
    }
    else if (value < 1000000000000)
    {
        str = QString::number(value/1000000000) + " " + "G";
    }

    return str;
}

QString lneUnit::UnitChangeForNeg(double value)
{
    QString str;

    if (value > pow(10,0))
    {
        str = QString::number(value);
    }
    else if (value > pow(10,-3))
    {
        str = QString::number(value*1000) + " " + "m";
    }
    else if (value > pow(10,-6))
    {
        str = QString::number(value*1000000) + " " + "u";
    }
    else if (value > pow(10,-9))
    {
        str = QString::number(value*1000000000) + " " + "n";
    }

    return str;
}
