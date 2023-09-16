#ifndef BTNBOOL_H
#define BTNBOOL_H

#include <QObject>
#include <QPushButton>
#include <QDebug>

class BtnBool: public QPushButton
{    
    Q_OBJECT
public:
    explicit BtnBool(QWidget *parent = nullptr);

    void on_set_text();
    void on_clicked();
    void on_get_state(bool * state);
    bool on_get_state();
private:
    bool m_state = false;

};

#endif // BTNBOOL_H
