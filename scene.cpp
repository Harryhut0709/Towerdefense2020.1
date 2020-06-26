#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPainter"
#include "QPixmap"
#include "QPushButton"
#include "button.h"
#include "scene.h"


Scene::Scene(QWidget *parent) : QMainWindow(parent)
{
    this->setFixedSize(960,640);
    Button * setting1btn = new Button(":/image/startbutton.png");
    Button * setting2btn = new Button(":/image/startbutton.png");
    setting1btn->setParent(this);
    setting1btn->move(150,500);
    setting2btn->setParent(this);
    setting2btn->move(590,500);
//    MainWindow * mainwindow1 = new MainWindow;   //-

    connect(setting1btn, &QPushButton::clicked,this,[=](){
       this->hide();
       MainWindow1 * mainwindow1 = new MainWindow1;  //+ //修复了两个界面同时构造的问题，在点击按钮后才会进入游戏界面，此时游戏才开始
       mainwindow1->show();
    });//lambda函数
    connect(setting2btn, &QPushButton::clicked,this,[=](){
       this->hide();
       MainWindow2 * mainwindow2 = new MainWindow2;  //+ //修复了两个界面同时构造的问题，在点击按钮后才会进入游戏界面，此时游戏才开始
       mainwindow2->show();
    });//lambda函数
}

void Scene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(":/image/startscene.png"));
    painter.drawPixmap(130,300,QPixmap(":/image/setting1button.png"));
    painter.drawPixmap(565,300,QPixmap(":/image/setting2button.png"));
    painter.drawPixmap(350,40,QPixmap(":/image/title.png"));
}
