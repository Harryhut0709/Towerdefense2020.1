#include "tower.h"
#include "enemy.h"
#include "bullet.h"
#include "mainwindow.h"
#include "utility.h"
#include <QPainter>
#include <QColor>
#include <QTimer>
#include <QVector2D>
#include <QtMath>

const QSize Tower::ms_fixedSize(42, 42);

Tower::Tower(QObject *parent) : QObject(parent)
{}

Tower::Tower(QPoint pos, MainWindow *game, const QPixmap &sprite /* =  QPixmap(":/image/tower1.png") */)
    : m_attacking(false)
    , m_attackRange(150)
    , m_damage(8)
    , m_fireRate(1200)
    , m_rotationSprite(0.0)
    , m_chooseEnemy(NULL)
    , m_game(game)
    , m_pos(pos)
    , m_sprite(sprite)
{
//    m_towerType=1;
    m_fireRateTimer = new QTimer(this);
    connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

void Tower::draw(QPainter *painter) const
{
    //����Ҫ�ı�painter��һЩ���ã����Լ���save/restore�����ʹ��
    painter->save();
    painter->setPen(Qt::white);
    //���ƹ�����Χ
    painter->drawEllipse(m_pos,m_attackRange,m_attackRange);

    //����ƫת���꣬������+ƫ��=����
    static const QPoint offsetPoint(-ms_fixedSize.width()/2, -ms_fixedSize.height()/2-15);

    //���Ʋ�ѡ������
    //���ｫ����ԭ���ƶ���m_pos
    painter->translate(m_pos);
    painter->rotate(m_rotationSprite+180);//
    painter->drawPixmap(offsetPoint,m_sprite);
    painter->restore();
}

void Tower::checkEnemyInRange()
{
    if (m_chooseEnemy)
    {
        // ���������,��Ҫ��ת��̨��׼����
        // ������׼��
        QVector2D normalized(m_chooseEnemy->pos() - m_pos);
        normalized.normalize();
        m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) - 90;

        // ����������빥����Χ
        if (!collisionWithCircle(m_pos, m_attackRange, m_chooseEnemy->pos(), 1))
            lostSightOfEnemy();
    }
    else
    {
        // ��������,���Ƿ��е����ڹ�����Χ��
        QList<Enemy *> enemyList = m_game->enemyList();
        foreach (Enemy *enemy, enemyList)
        {
            if (collisionWithCircle(m_pos, m_attackRange, enemy->pos(), 1))
            {
                chooseEnemyForAttack(enemy);
                break;
            }
        }
    }
}

void Tower::targetKilled()
{
    //Ŀ������ʱȡ��������ֹͣ����
    if (m_chooseEnemy)
        m_chooseEnemy = NULL;

    m_fireRateTimer->stop();
    m_rotationSprite = 0.0;//������λ
}

void Tower::attackEnemy()
{
    //��������ģʽ
    m_fireRateTimer->start(m_fireRate);
}

void Tower::chooseEnemyForAttack(Enemy *enemy)
{
    //ѡ����ˣ����Ե��˽��й���
    m_chooseEnemy = enemy;
    //����Timer����������
    attackEnemy();
    //�����빥���߹�������QList�������ߣ���Ϊ����ͬʱ�ж�������ڹ���ͬһ������
    m_chooseEnemy->getAttacked(this);
}
//������������������һ�����ֵĵ��ˣ�ֱ������Ϊֹ

void Tower::lostSightOfEnemy()
{
    //�����ܳ��������Ĺ�����Χ����Ҫȡ��������˵Ĺ�����ͬʱֹͣ����
    m_chooseEnemy->gotLostSight(this);
    if (m_chooseEnemy)
        m_chooseEnemy = NULL;

    m_fireRateTimer->stop();
    m_rotationSprite = 0.0;//������λ
}

void Tower::shootWeapon()
{
    //ÿ�ι�������һ���ӵ����ӵ������󽻸�m_game���й�������
    Bullet *bullet = new Bullet(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
    bullet->move();//����moveִ�ж���Ч��
    m_game->addBullet(bullet);
}

Tower::~Tower()
{
    delete m_fireRateTimer;
    m_fireRateTimer = NULL;
}
