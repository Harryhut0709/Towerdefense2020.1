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
    MainWindow * mainwindow1 = new MainWindow;
    connect(btn, &QPushButton::clicked,this,[=](){
       this->hide();
       mainwindow1->show();
    });//lambdaº¯Êý
}

void Scene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(":/image/startscene.png"));
}
