//这个类是对敌方怪兽的声明
#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QPainter>

#include "mainwindow.h"
#include "waypoint.h"
#include "tower.h"

class Enemy : public QObject
{
    Q_OBJECT
public:
    explicit Enemy(QObject *parent = nullptr);
    Enemy(Waypoint *startWayPoint, MainWindow *game,int level = 1,
          const QPixmap &sprite = QPixmap(":/image/enemy1.png"),
          int maxHp = 40,qreal walkingSpeed = 2.0);
    ~Enemy();
    //设置为虚函数
    virtual void draw(QPainter *painter) const;     //画怪兽
    virtual void move();                            //怪兽根据航点顺序移动
    virtual void getDamage(int damage);             //受到伤害
    virtual void getRemoved();                      //打死后怪兽消失
    virtual void getAttacked(Tower *attacker);      //进入塔的攻击范围，被锁定
    virtual void gotLostSight(Tower *attacker);     //离开塔的攻击范围，解除锁定
    virtual void setLevel(int level);               //设置怪兽等级
    virtual int  getCurrentHp() const;              //+ //判断是被打死了还是走到基地了，加声音

    //未实现
    void slowDown();                                //被icebullet击中，修改m_isSlowed的值为1
    void getPoisoned();                             //被poisonbullet击中，怪兽一段时间的持续伤害
    //+

    QPoint pos() const;                             //传回怪兽当前位置

public slots:
    void doActivate();                              //槽函数，控制怪兽沿waypoints移动
    void SpeedUp();                                 //槽函数，修改m_isSlowed的值为0 //+
    void CurePoison();                              //槽函数，修改m_isPoisoned的值为1 //+
    void getPoisonDamage();                         //槽函数，中毒持续伤害 //+

protected://为继承做准备
    bool			m_active;                 //表示是否可以移动，只有当其为true时，敌人才可以移动
    bool            m_isSlowed=0;             //被icebullet击中，怪兽一段时间的减速 //默认为0，被击中后一段时间此值为1，造成冰冻减速
    bool            m_isPoisoned=0;           //被poisonbullet击中，怪兽一段时间的持续伤害 //默认为0，被击中一段时间后此值为1，造成持续中毒伤害

    int				m_maxHp;                  //最大血量
    int				m_currentHp;              //当前血量
    int             m_level;                  //敌人等级
    qreal			m_walkingSpeed;           //qreal类型相当于double
    qreal			m_rotationSprite;         //存储敌人到下一个航点时图片的旋转角度

    Waypoint *		m_destinationWayPoint;    //用来存储当前航点
    MainWindow *	m_game;                   //传回MainWindow实现paintevent
    QList<Tower *>	m_attackedTowersList;     //与敌人目标相连的正在攻击的塔（因为可能同时有多个塔在打一个敌人，所以需要enemy来维护攻击塔的QList）

    //常规三件套
    QPoint			m_pos;
    const QPixmap	m_sprite;
    static const QSize ms_fixedSize;
signals:

};

#endif // ENEMY_H
