#include "bullet.h"
#include "enemy.h"
#include "mainwindow.h"
#include <QPainter>
#include <QPropertyAnimation>

const QSize Bullet::ms_fixedSize(32, 32);  //ԭ�ߴ磬֮����Ե���

Bullet::Bullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
               MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/normalbullet.png")*/)
    : m_startPos(startPos)
    , m_targetPos(targetPoint)
    , m_sprite(sprite)
    , m_currentPos(startPos)
    , m_target(target)
    , m_game(game)
    , m_damage(damage)
{}//���캯�������ó�ʼ���б�

void Bullet::draw(QPainter *painter) const
{
    painter->drawPixmap(m_currentPos, m_sprite);
}

void Bullet::move()
{
    // 150�����ڻ��е���
    static const int duration = 150;
    //����QT�Դ���animation����ʵ���ӵ��Ķ���Ч��
    QPropertyAnimation *animation = new QPropertyAnimation(this, "m_currentPos");

    animation->setDuration(duration);
    animation->setStartValue(m_startPos);
    animation->setEndValue(m_targetPos);
    connect(animation, SIGNAL(finished()), this, SLOT(hitTarget()));
    //��������������Ŀ�����hitTarget

    animation->start();
}

void Bullet::hitTarget()
{
    // ���������ԭ����:���ܶ���ڵ����е��ˣ�������һ���������𣬵��µ���delete�������ڵ��ٹ������ĵ��˾�����Ч�ڴ�����������ж��µ����Ƿ�û�б�����
    if (m_game->enemyList().indexOf(m_target) != -1)//indexof�������������û������
        {m_target->getDamage(m_damage);}//�ۼ�����Hp
    m_game->removedBullet(this);//�ӵ���ʧ
}

void Bullet::setCurrentPos(QPoint pos)
{
    m_currentPos = pos;
}

QPoint Bullet::currentPos() const
{
    return m_currentPos;
}


