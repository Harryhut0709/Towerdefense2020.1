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
    Button * btn = new Button(":/image/startbutton.png");
    btn->setParent(this);
    btn->move(350,400);
//    MainWindow * mainwindow1 = new MainWindow;   //-
    connect(btn, &QPushButton::clicked,this,[=](){
       this->hide();
       MainWindow * mainwindow1 = new MainWindow;  //+ //�޸�����������ͬʱ��������⣬�ڵ����ť��Ż������Ϸ���棬��ʱ��Ϸ�ſ�ʼ
       mainwindow1->show();
    });//lambda����
}

void Scene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(":/image/startscene.png"));
}
