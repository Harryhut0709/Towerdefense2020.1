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
    //由于要改变painter的一些设置，所以加上save/restore来配合使用
    painter->save();
    painter->setPen(Qt::white);
    //绘制攻击范围
    painter->drawEllipse(m_pos,m_attackRange,m_attackRange);

    //绘制偏转坐标，由中心+偏移=左上
    static const QPoint offsetPoint(-ms_fixedSize.width()/2, -ms_fixedSize.height()/2-15);
    static const QPoint upgradeoffsetpoint(10,-25);//升级按钮偏移量 //+
    static const QPoint selloffsetpoint(10,5);//拆除按钮偏移量 //+

    //绘制并选择炮塔
    //这里将坐标原点移动到m_pos
    painter->translate(m_pos);
    painter->drawPixmap(upgradeoffsetpoint,m_upgradebutton);//画升级按钮 //+
    painter->drawPixmap(selloffsetpoint,m_sellbutton);//画拆塔按钮 //+
    //这两步需要在rotate之前，否则按钮会跟着塔一起旋转
    painter->rotate(m_rotationSprite+180);//
    painter->drawPixmap(offsetPoint,m_sprite);
    painter->restore();
}

void IceTower::draw(QPainter *painter) const//+
{
    painter->save();
    painter->setPen(Qt::white);
    painter->drawEllipse(m_pos,m_attackRange,m_attackRange);
    static const QPoint offsetPoint(-ms_fixedSize.width()/2-10, -ms_fixedSize.height()/2-10);//只有这里偏转坐标需要改动
    static const QPoint upgradeoffsetpoint(10,-25);//升级按钮偏移量
    static const QPoint selloffsetpoint(10,5);//拆除按钮偏移量
    painter->translate(m_pos);
    painter->drawPixmap(upgradeoffsetpoint,m_upgradebutton);//画升级按钮
    painter->drawPixmap(selloffsetpoint,m_sellbutton);//画拆塔按钮
    painter->rotate(m_rotationSprite+180);
    painter->drawPixmap(offsetPoint,m_sprite);
    painter->restore();
}

void PoisonTower::draw(QPainter *painter) const//+
{
    painter->save();
    painter->setPen(Qt::white);
    painter->drawEllipse(m_pos,m_attackRange,m_attackRange);
    static const QPoint offsetPoint(-ms_fixedSize.width()/2+5, -ms_fixedSize.height()/2);//只有这里偏转坐标需要改动
    static const QPoint upgradeoffsetpoint(10,-25);//升级按钮偏移量
    static const QPoint selloffsetpoint(10,5);//拆除按钮偏移量
    painter->translate(m_pos);
    painter->drawPixmap(upgradeoffsetpoint,m_upgradebutton);//画升级按钮
    painter->drawPixmap(selloffsetpoint,m_sellbutton);//画拆塔按钮
    painter->rotate(m_rotationSprite+180);
    painter->drawPixmap(offsetPoint,m_sprite);
    painter->restore();
}

void Tower::checkEnemyInRange()
{
    if (m_chooseEnemy)
    {
        // 这种情况下,需要旋转炮台对准敌人
        // 向量标准化
        QVector2D normalized(m_chooseEnemy->pos() - m_pos);
        normalized.normalize();
        m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) - 90;

        // 如果敌人脱离攻击范围
        if (!collisionWithCircle(m_pos, m_attackRange, m_chooseEnemy->pos(), 1))
            lostSightOfEnemy();
    }
    else
    {
        // 遍历敌人,看是否有敌人在攻击范围内
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
    //目标死亡时取消关联，停止攻击
    if (m_chooseEnemy)
        m_chooseEnemy = NULL;

    m_fireRateTimer->stop();
    m_rotationSprite = 0.0;//炮塔归位
}

void Tower::attackEnemy()
{
    //开启攻击模式
    m_fireRateTimer->start(m_fireRate);
}

void Tower::chooseEnemyForAttack(Enemy *enemy)
{
    //选择敌人，并对敌人进行攻击
    m_chooseEnemy = enemy;
    //启动Timer，发动攻击
    attackEnemy();
    //敌人与攻击者关联，用QList管理攻击者，因为可能同时有多个炮塔在攻击同一个敌人
    m_chooseEnemy->getAttacked(this);
}
//炮塔攻击规则：锁定第一个发现的敌人，直到打死为止

void Tower::lostSightOfEnemy()
{
    //敌人跑出攻击塔的攻击范围，需要取消塔与敌人的关联，同时停止攻击
    m_chooseEnemy->gotLostSight(this);
    if (m_chooseEnemy)
        m_chooseEnemy = NULL;

    m_fireRateTimer->stop();
    m_rotationSprite = 0.0;//炮塔归位
}

void Tower::shootWeapon()
{
    //每次攻击产生一颗子弹，子弹产生后交给m_game进行管理并绘制
    Bullet *bullet = new Bullet(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
    bullet->move();//调用move执行动画效果
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

//+ //升级和拆除
void Tower::setUpgradeTower()
{
    this->m_towerLevel=2;
}

bool Tower::isMaxlevel() const
{
    if (m_towerLevel==2) return true;//已经满级
    else return false;//未满级，可升级
}

const QPoint Tower::upgradebuttoncenterPos() const//升级塔按钮的中心点
{
    static const QPoint upgradeoffsetpoint(10,-25);//draw()函数中的偏移量
    return m_pos+upgradeoffsetpoint;
}

const QPoint Tower::sellbuttoncenterPos() const//卖塔按钮的中心点
{
    static const QPoint selloffsetpoint(10,5);//draw()函数中的偏移量
    return m_pos+selloffsetpoint;
}


bool Tower::containupgradebutton(const QPoint &pos) const//是否在升级按钮区域内
{
    bool isXInHere = upgradebuttoncenterPos().x() < pos.x() && pos.x() < (upgradebuttoncenterPos().x() + ms_buttonfixedSize.width());
    bool isYInHere = upgradebuttoncenterPos().y() < pos.y() && pos.y() < (upgradebuttoncenterPos().y() + ms_buttonfixedSize.height());
    return isXInHere && isYInHere;
}

bool Tower::containsellbutton(const QPoint &pos) const//是否在卖塔按钮区域内
{
    bool isXInHere = sellbuttoncenterPos().x() < pos.x() && pos.x() < (sellbuttoncenterPos().x() + ms_buttonfixedSize.width());
    bool isYInHere = sellbuttoncenterPos().y() < pos.y() && pos.y() < (sellbuttoncenterPos().y() + ms_buttonfixedSize.height());
    return isXInHere && isYInHere;
}

