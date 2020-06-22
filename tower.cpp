#include "tower.h"
#include "enemy.h"
#include "bullet.h"
#include "mainwindow.h"
#include "utility.h"
#include "button.h"
#include <QPainter>
#include <QColor>
#include <QTimer>
#include <QVector2D>
#include <QtMath>

const QSize Tower::ms_fixedSize(42, 42);
const QSize Tower::ms_buttonfixedSize(20, 20);//+

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
    , m_upgradebutton(":/image/upgradetowerbutton.png")
    , m_sellbutton(":/image/selltowerbutton.png")
    , m_pos(pos)
    , m_sprite(sprite)
{
    m_towerType=1; //+
    m_towerLevel=1; //++
    m_fireRateTimer = new QTimer(this);
    connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

IceTower::IceTower(QPoint pos, MainWindow *game,const QPixmap &sprite /* = QPixmap(":/image/tower2.png") */)
    :Tower(pos,game,sprite)//+
{
    m_towerType=2;
    m_attackRange=150;
    m_damage=5;
    m_fireRate=1000;
    m_fireRateTimer = new QTimer(this);
    connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

PoisonTower::PoisonTower(QPoint pos, MainWindow *game,const QPixmap &sprite /* = QPixmap(":/image/tower3.png") */)
    :Tower(pos,game,sprite)//+
{
    m_towerType=3;
    m_attackRange=150;
    m_damage=5;
    m_fireRate=1500;
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
    static const QPoint upgradeoffsetpoint(10,-25);//������ťƫ���� //+
    static const QPoint selloffsetpoint(10,5);//�����ťƫ���� //+

    //���Ʋ�ѡ������
    //���ｫ����ԭ���ƶ���m_pos
    painter->translate(m_pos);
    painter->drawPixmap(upgradeoffsetpoint,m_upgradebutton);//��������ť //+
    painter->drawPixmap(selloffsetpoint,m_sellbutton);//��������ť //+
    //��������Ҫ��rotate֮ǰ������ť�������һ����ת
    painter->rotate(m_rotationSprite+180);//
    painter->drawPixmap(offsetPoint,m_sprite);
    painter->restore();
}

void IceTower::draw(QPainter *painter) const//+
{
    painter->save();
    painter->setPen(Qt::white);
    painter->drawEllipse(m_pos,m_attackRange,m_attackRange);
    static const QPoint offsetPoint(-ms_fixedSize.width()/2-10, -ms_fixedSize.height()/2-10);//ֻ������ƫת������Ҫ�Ķ�
    static const QPoint upgradeoffsetpoint(10,-25);//������ťƫ����
    static const QPoint selloffsetpoint(10,5);//�����ťƫ����
    painter->translate(m_pos);
    painter->drawPixmap(upgradeoffsetpoint,m_upgradebutton);//��������ť
    painter->drawPixmap(selloffsetpoint,m_sellbutton);//��������ť
    painter->rotate(m_rotationSprite+180);
    painter->drawPixmap(offsetPoint,m_sprite);
    painter->restore();
}

void PoisonTower::draw(QPainter *painter) const//+
{
    painter->save();
    painter->setPen(Qt::white);
    painter->drawEllipse(m_pos,m_attackRange,m_attackRange);
    static const QPoint offsetPoint(-ms_fixedSize.width()/2+5, -ms_fixedSize.height()/2);//ֻ������ƫת������Ҫ�Ķ�
    static const QPoint upgradeoffsetpoint(10,-25);//������ťƫ����
    static const QPoint selloffsetpoint(10,5);//�����ťƫ����
    painter->translate(m_pos);
    painter->drawPixmap(upgradeoffsetpoint,m_upgradebutton);//��������ť
    painter->drawPixmap(selloffsetpoint,m_sellbutton);//��������ť
    painter->rotate(m_rotationSprite+180);
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

void Tower::setTowerLevel()
{
    if (m_towerLevel==1)
    {
        m_attackRange=150;
        m_damage=8;
        m_fireRate=1200;
    }
    if (m_towerLevel==2)
    {
        m_attackRange=200;
        m_damage=10;
        m_fireRate=100;
    }
}

int Tower::getTowerType()
{
    return m_towerType;
}

Tower::~Tower()
{
    delete m_fireRateTimer;
    m_fireRateTimer = NULL;
}

IceTower::~IceTower()//+
{
    delete m_fireRateTimer;
    m_fireRateTimer = NULL;
}

PoisonTower::~PoisonTower()//+
{
    delete m_fireRateTimer;
    m_fireRateTimer = NULL;
}

void IceTower::shootWeapon()
{
    Bullet *bullet = new IceBullet(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
    bullet->move();
    m_game->addBullet(bullet);
}

void PoisonTower::shootWeapon()
{
    Bullet *bullet = new PoisonBullet(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
    bullet->move();
    m_game->addBullet(bullet);
}

//+ //�����Ͳ��
void Tower::setUpgradeTower()
{
    this->m_towerLevel=2;
}

bool Tower::isMaxlevel() const
{
    if (m_towerLevel==2) return true;//�Ѿ�����
    else return false;//δ������������
}

const QPoint Tower::upgradebuttoncenterPos() const//��������ť�����ĵ�
{
    static const QPoint upgradeoffsetpoint(10,-25);//draw()�����е�ƫ����
    return m_pos+upgradeoffsetpoint;
}

const QPoint Tower::sellbuttoncenterPos() const//������ť�����ĵ�
{
    static const QPoint selloffsetpoint(10,5);//draw()�����е�ƫ����
    return m_pos+selloffsetpoint;
}


bool Tower::containupgradebutton(const QPoint &pos) const//�Ƿ���������ť������
{
    bool isXInHere = upgradebuttoncenterPos().x() < pos.x() && pos.x() < (upgradebuttoncenterPos().x() + ms_buttonfixedSize.width());
    bool isYInHere = upgradebuttoncenterPos().y() < pos.y() && pos.y() < (upgradebuttoncenterPos().y() + ms_buttonfixedSize.height());
    return isXInHere && isYInHere;
}

bool Tower::containsellbutton(const QPoint &pos) const//�Ƿ���������ť������
{
    bool isXInHere = sellbuttoncenterPos().x() < pos.x() && pos.x() < (sellbuttoncenterPos().x() + ms_buttonfixedSize.width());
    bool isYInHere = sellbuttoncenterPos().y() < pos.y() && pos.y() < (sellbuttoncenterPos().y() + ms_buttonfixedSize.height());
    return isXInHere && isYInHere;
}

