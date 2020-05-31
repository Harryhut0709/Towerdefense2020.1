//这个类之后慢慢实现
//希望实现选择模式难易
//希望实现主界面和游戏界面的切换
//希望实现游戏的暂停和重新开始

#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QPushButton>

class Button : public QPushButton
{
    Q_OBJECT
public:
    Button(QString pics);
signals:

};

#endif // BUTTON_H
