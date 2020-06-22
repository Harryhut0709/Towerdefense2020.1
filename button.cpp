#include "button.h"
#include <QPixmap>
#include <QSize>

Button::Button(QString pics): QPushButton(0)
{
    QPixmap pixmap(pics);
    this->setFixedSize(pixmap.width(),pixmap.height());
    this->setStyleSheet("QPushButton{border:0px;}");//把按钮变成不规则形状
    this->setIcon(pixmap);
    this->setIconSize(QSize(pixmap.width(),pixmap.height()));
}
