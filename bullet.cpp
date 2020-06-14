#include "bullet.h"
#include "enemy.h"
#include "mainwindow.h"
#include <QPainter>
#include <QPropertyAnimation>

const QSize Bullet::ms_fixedSize(32, 32);  //原尺寸，之后可以调整

Bullet::Bullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
               MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/normalbullet.png")*/)
    : m_startPos(startPos)
    , m_targetPos(targetPoint)
    , m_sprite(sprite)
    , m_currentPos(startPos)
    , m_target(target)
    , m_game(game)
    , m_damage(damage)
{}//构造函数，利用初始化列表

//+
IceBullet::IceBullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
                     MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/icebullet.png")*/)
    : Bullet(startPos,targetPoint,damage,target,game,sprite)
{}

IceBullet::~IceBullet()
{}

PoisonBullet::PoisonBullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
                          MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/poisonbullet.png")*/)
    : Bullet(startPos,targetPoint,damage,target,game,sprite)
{}

PoisonBullet::~PoisonBullet()
{}

//+

void Bullet::draw(QPainter *painter) const
{
    painter->drawPixmap(m_currentPos, m_sprite);
}

void Bullet::move()
{
    // 150毫秒内击中敌人
    static const int duration = 150;
    //利用QT自带的animation方法实现子弹的动画效果
    QPropertyAnimation *animation = new QPropertyAnimation(this, "m_currentPos");

    animation->setDuration(duration);
    animation->setStartValue(m_startPos);
    animation->setEndValue(m_targetPos);
    connect(animation, SIGNAL(finished()), this, SLOT(hitTarget()));
    //动画结束，关联目标敌人hitTarget

    animation->start();
}

void Bullet::hitTarget()
{
    // 这样处理的原因是:可能多个炮弹击中敌人，而其中一个将其消灭，导致敌人delete，后续炮弹再攻击到的敌人就是无效内存区域，因此先判断下敌人是否还没有被打死
    if (m_game->enemyList().indexOf(m_target) != -1)//indexof方法，如果敌人没被打死
        {m_target->getDamage(m_damage);}//扣减敌人Hp
    m_game->removedBullet(this);//子弹消失
}

void IceBullet::hitTarget()//+
{
    if (m_game->enemyList().indexOf(m_target) != -1)//indexof方法，如果敌人没被打死
    {
        m_target->slowDown();//m_isSlowed变为1
        m_target->getDamage(m_damage);//扣减敌人Hp
    }
    m_game->removedBullet(this);//子弹消失
}

void PoisonBullet::hitTarget()//+
{
    if (m_game->enemyList().indexOf(m_target) != -1)//indexof方法，如果敌人没被打死
    {
        m_target->getPoisoned();//m_isPoisoned变为1
        m_target->getDamage(m_damage);//扣减敌人Hp
    }
    m_game->removedBullet(this);//子弹消失
}

void Bullet::setCurrentPos(QPoint pos)
{
    m_currentPos = pos;
}

QPoint Bullet::currentPos() const
{
    return m_currentPos;
}


