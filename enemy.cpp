#include "enemy.h"
#include "utility.h"

static const int Health_Bar_Width = 40;

const QSize Enemy::ms_fixedSize(110, 42);

Enemy::Enemy(QObject *parent) : QObject(parent){}

Enemy::Enemy(Waypoint *startWayPoint, MainWindow *game,int level /*= 1 */,
             const QPixmap &sprite /* = QPixmap(":/image/enemy1.png") */,
             int maxHp /* = 40 */,qreal walkingSpeed /*= 2.0 */)
    : QObject(0)
    , m_active(false)              //注意这个初始化
    , m_maxHp(maxHp)
    , m_currentHp(maxHp)
    , m_walkingSpeed(walkingSpeed)
    , m_level(level)
    , m_rotationSprite(0.0)
    , m_pos(startWayPoint->pos())
    , m_destinationWayPoint(startWayPoint->nextWayPoint())  //注意这个初始化
    , m_game(game)
    , m_sprite(sprite)
{
//    setLevel(m_level);//这是啥？？？？
}

void Enemy::draw(QPainter *painter) const
{
    if (!m_active)
        return;

    painter->save();

    //血条的长度
    //两个方框，红色方框代表总生命，固定大小不变
    //绿色方框代表当前剩余生命，受m_currentHp / m_maxHp的变化影响
    //一看就是老塔防玩家了
    QPoint healthBarPoint = m_pos + QPoint(-Health_Bar_Width / 2 - 5, -ms_fixedSize.height() / 3) - QPoint(0,20);//最后一个参数是血条偏移
    // 绘制血条
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    QRect healthBarBackRect(healthBarPoint, QSize(Health_Bar_Width, 2));
    painter->drawRect(healthBarBackRect);

    painter->setBrush(Qt::green);
    QRect healthBarRect(healthBarPoint, QSize((double)m_currentHp / m_maxHp * Health_Bar_Width, 2));
    painter->drawRect(healthBarRect);

    // 绘制偏转坐标,由中心+偏移=左上
    static const QPoint offsetPoint(-ms_fixedSize.width() / 2, -ms_fixedSize.height() / 2);
    painter->translate(m_pos);
    painter->rotate(m_rotationSprite);//这一步是之前的draw方法没有遇到过的，实现旋转

    // 绘制敌人
    painter->drawPixmap(offsetPoint, m_sprite);
    painter->restore();
}


void Enemy::move()
{
    if (!m_active)//没有激活的怪不能动
        return;

    if (collisionWithCircle(m_pos, 1, m_destinationWayPoint->pos(), 1))
    {
        // 敌人抵达了一个航点
        if (m_destinationWayPoint->nextWayPoint())
        {
            // 还有下一个航点
            m_pos = m_destinationWayPoint->pos();
            m_destinationWayPoint = m_destinationWayPoint->nextWayPoint();
        }
        else
        {
            // 表示进入基地
            m_game->getHpDamage();
            m_game->removedEnemy(this);
            return;
        }
    }

    // 还在前往航点的路上
    // 目标航点的坐标
    QPoint targetPoint = m_destinationWayPoint->pos();

    // 未来修改这个可以添加移动状态,加快,减慢,m_walkingSpeed是基准
    qreal movementSpeed = m_walkingSpeed;
    // 向量标准化
    // 移动速度其实每次都是1,normalized取值只有(1,0)/(-1,0)/(0,1)/(0,-1)四种,主要用来计算敌人旋转角度(0/90/180/270)
    QVector2D normalized(targetPoint - m_pos);
    normalized.normalize();
    m_pos = m_pos + normalized.toPoint() * movementSpeed;

    // 确定敌人选择方向
    // 默认图片向左,需要修正180度转右
    m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x()));
}

void Enemy::getDamage(int damage)
{
//    m_game->audioPlayer()->playSound(LaserShootSound);
    m_currentHp -= damage;//扣减敌人收到的伤害

    // 阵亡,需要移除
    if (m_currentHp <= 0)
    {
//        m_game->audioPlayer()->playSound(EnemyDestorySound);
        m_game->awardGold(200);//打死一个敌人奖励金币
        getRemoved();//敌人消失
    }
}

void Enemy::getRemoved()
{
    if (m_attackedTowersList.empty())
        return;

    foreach (Tower *attacker, m_attackedTowersList)
        attacker->targetKilled();
    // 通知game,此敌人已经阵亡
    m_game->removedEnemy(this);
}

void Enemy::getAttacked(Tower *attacker)
{
    m_attackedTowersList.push_back(attacker);
}

// 表明敌人已经逃离了攻击范围

void Enemy::gotLostSight(Tower *attacker)
{
    m_attackedTowersList.removeOne(attacker);
}

void Enemy::setLevel(int level){}

QPoint Enemy::pos() const
{
    return m_pos;
}

void Enemy::doActivate()
{
    m_active = true;
}
//调用这个槽函数之后，enemy才可以行动

Enemy::~Enemy(){}


