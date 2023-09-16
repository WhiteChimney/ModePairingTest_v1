#include "btnbool.h"

BtnBool::BtnBool(QWidget *parent)
    : QPushButton(parent)
{
    connect(this,&QPushButton::clicked,this,&BtnBool::on_clicked);
    this->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(211, 211, 211);");

//    this->setStyleSheet("border-style: outset; border-width: 2px; border-radius: 5px; "
//                        "color: rgb(0, 0, 0); background-color: rgb(0, 255, 0);");
}

void BtnBool::on_set_text()
{
    this->setText("STOP");
}

void BtnBool::on_clicked()
{
    m_state = !m_state;
    if (m_state == false)
    {
//        this->setText("STOP");
        this->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(211, 211, 211);");
    }
    else
    {
//        this->setText("STOP");
        this->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(0, 255, 0);");
    }

}


void BtnBool::on_get_state(bool * state)
{
    * state = m_state;
}

bool BtnBool::on_get_state()
{
    return m_state;
}
