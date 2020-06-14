//目前只有一种普通子弹normalbullet
//之后预计加入两种特殊的子弹：冰冻子弹icebullet（减速）、有毒子弹poisonbullet（持续伤害）

#ifndef BULLET_H
#define BULLET_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>

class QPainter;
class Enemy;
class MainWindow;

class Bullet :public QObject
{
    Q_OBJECT//所有应用QT槽的类都需要声明
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)//QT动画效果
    //注册为QT属性，在子弹生成后调用move()函数，使得子弹进行自动动画效果

public:
    Bullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/normalbullet.png"));

    void draw(QPainter *painter) const;       //画子弹
    void move();                              //实现子弹动画效果，利用qt自带的animation方法
    void setCurrentPos(QPoint pos);           //更新子弹实时位置

    QPoint currentPos() const;                //获取子弹实时位置

public slots:
    virtual void hitTarget();                 //槽函数：击中敌人时与敌人相连接，使得敌人受到伤害

protected:
    const QPoint	m_startPos;               //炮塔发出位置（子弹起始点）
    const QPoint	m_targetPos;              //敌人位置（子弹目的地）
    const QPixmap	m_sprite;                 //子弹样式
    QPoint			m_currentPos;             //当前位置（不断更新）
    Enemy *			m_target;                 //要攻击的敌人
    MainWindow *	m_game;                   //传回MainWindow实现paintevent
    int				m_damage;                 //由攻击塔的属性决定

    static const QSize ms_fixedSize;
};

//+
class IceBullet:public Bullet{
    Q_OBJECT//所有应用QT槽的类都需要声明
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
    IceBullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/icebullet.png"));
    ~IceBullet();

public slots:
    void hitTarget();                 //槽函数：击中敌人时与敌人相连接，使得敌人受到伤害
    //同时改变被击中被击中的enemy的m_isSlowed
};

class PoisonBullet:public Bullet{
    Q_OBJECT//所有应用QT槽的类都需要声明
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
    PoisonBullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/poisonbullet.png"));
    ~PoisonBullet();

public slots:
    void hitTarget();                 //槽函数：击中敌人时与敌人相连接，使得敌人受到伤害
    //同时改变被击中被击中的enemy的m_isPoisoned
};

#endif // BULLET_H
