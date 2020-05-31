//这个类是游戏开始界面
#ifndef SCENE_H
#define SCENE_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>

class Scene : public QMainWindow
{
    Q_OBJECT
public:
    explicit Scene(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);

signals:

};

#endif // SCENE_H
