#include "enemy.h"
#include "utility.h"
#include <QTimer> //+ //用于控制冰冻时间
#include <QObject> //+ //调用connect函数

static const int Health_Bar_Width = 40;

const QSize Enemy::ms_fixedSize(110, 42);

Enemy::Enemy(QObject *parent) : QObject(parent){}

Enemy::Enemy(Waypoint *startWayPoint, MainWindow *game,int level /*= 1 */,
             const QPixmap &sprite /* = QPixmap(":/image/enemy1.png") */,
             int maxHp /* = 40 */,qreal walkingSpeed /*= 2.0 */)
    : QObject(0)
    , m_active(false)             //注意这个初始化
    , m_isSlowed(false)
    , m_isPoisoned(false)         //+ //初始时均false,被相关子弹击中后一段时间受到相应伤害
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
    // +
    if(m_isSlowed)//被icebullet击中,减速为原来的一半，持续时间可以修改
    {
        //引入一个timer控制冰冻时间
        QTimer *frozentimer = new QTimer();
        frozentimer->setInterval(2000);//被冰冻的时间
        connect(frozentimer,SIGNAL(timeout()),this,SLOT(SpeedUp()));
        frozentimer->start();
        qreal movementSpeed = 0.3 * m_walkingSpeed;
        // 向量标准化
        // 移动速度其实每次都是1,normalized取值只有(1,0)/(-1,0)/(0,1)/(0,-1)四种,主要用来计算敌人旋转角度(0/90/180/270)
        QVector2D normalized(targetPoint - m_pos);
        normalized.normalize();
        m_pos = m_pos + normalized.toPoint() * movementSpeed;
        m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x()));
        // 确定敌人选择方向
        // 默认图片向左,需要修正180度转右
    }

    else
    {
        qreal movementSpeed = m_walkingSpeed;
        QVector2D normalized(targetPoint - m_pos);
        normalized.normalize();
        m_pos = m_pos + normalized.toPoint() * movementSpeed;
        m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x()));
    }

}

void Enemy::getDamage(int damage)
{
    m_currentHp -= damage;//扣减敌人收到的伤害

    if (m_isPoisoned)//+
    {
        QTimer *stoptimer = new QTimer();//这个用来控制结束时间
        QTimer *poisontimer = new QTimer();
        poisontimer->setInterval(1000);//每次中毒伤害时间
        stoptimer->setInterval(5000);//持续中毒时间
        stoptimer->setSingleShot(true);//只触发一次
        connect(poisontimer,SIGNAL(timeout()),this,SLOT(getPoisonDamage()));
        connect(stoptimer,SIGNAL(timeout()),this,SLOT(CurePoison()));
        poisontimer->start();
        stoptimer->start();
    }

    // 阵亡,需要移除
    if (m_currentHp <= 0)
    {
        m_game->awardGold(200);//打死一个敌人奖励金币
        getRemoved();//敌人消失
    }
}

void Enemy::getRemoved()//++ //加上了中毒效果，此处需要修改逻辑 //好像并没有找到正确的逻辑解决中毒bug...
{
    if (m_attackedTowersList.empty() && m_currentHp>0 )
        return;

    if (m_currentHp<=0 && m_isPoisoned==1)
    {
//        foreach (Tower *attacker, m_attackedTowersList)
//            attacker->targetKilled();
//        // 通知game,此敌人已经阵亡
        m_game->removedEnemy(this);
    }

    else if (m_currentHp<=0 && m_isPoisoned==0)
    {
                foreach (Tower *attacker, m_attackedTowersList)
                    attacker->targetKilled();
                // 通知game,此敌人已经阵亡
                m_game->removedEnemy(this);
    }
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

int Enemy::getCurrentHp() const
{
    return m_currentHp;
}

void Enemy::slowDown()//+ //被冰冻 //状态转换
{
    m_isSlowed=true;
}

void Enemy::SpeedUp()//+ //冰冻解除 //这必须是槽函数！！！！
{
    m_isSlowed=false;
}

void Enemy::getPoisoned()//+ //中毒
{
    m_isPoisoned=true;
}

void Enemy::CurePoison()//+ //中毒解除 //槽函数
{
    m_isPoisoned=false;
}

void Enemy::getPoisonDamage()//+ //中毒伤害 //槽函数
{
    m_currentHp -= 10;
}

void Enemy::doActivate()
{
    m_active = true;
}
//调用这个槽函数之后，enemy才可以行动

Enemy::~Enemy(){}
