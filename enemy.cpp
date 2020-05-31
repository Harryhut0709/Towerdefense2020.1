#include "enemy.h"
#include "utility.h"

static const int Health_Bar_Width = 40;

const QSize Enemy::ms_fixedSize(110, 42);

Enemy::Enemy(QObject *parent) : QObject(parent){}

Enemy::Enemy(Waypoint *startWayPoint, MainWindow *game,int level /*= 1 */,
             const QPixmap &sprite /* = QPixmap(":/image/enemy1.png") */,
             int maxHp /* = 40 */,qreal walkingSpeed /*= 2.0 */)
    : QObject(0)
    , m_active(false)              //ע�������ʼ��
    , m_maxHp(maxHp)
    , m_currentHp(maxHp)
    , m_walkingSpeed(walkingSpeed)
    , m_level(level)
    , m_rotationSprite(0.0)
    , m_pos(startWayPoint->pos())
    , m_destinationWayPoint(startWayPoint->nextWayPoint())  //ע�������ʼ��
    , m_game(game)
    , m_sprite(sprite)
{
//    setLevel(m_level);//����ɶ��������
}

void Enemy::draw(QPainter *painter) const
{
    if (!m_active)
        return;

    painter->save();

    //Ѫ���ĳ���
    //�������򣬺�ɫ����������������̶���С����
    //��ɫ�������ǰʣ����������m_currentHp / m_maxHp�ı仯Ӱ��
    //һ�����������������
    QPoint healthBarPoint = m_pos + QPoint(-Health_Bar_Width / 2 - 5, -ms_fixedSize.height() / 3) - QPoint(0,20);//���һ��������Ѫ��ƫ��
    // ����Ѫ��
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    QRect healthBarBackRect(healthBarPoint, QSize(Health_Bar_Width, 2));
    painter->drawRect(healthBarBackRect);

    painter->setBrush(Qt::green);
    QRect healthBarRect(healthBarPoint, QSize((double)m_currentHp / m_maxHp * Health_Bar_Width, 2));
    painter->drawRect(healthBarRect);

    // ����ƫת����,������+ƫ��=����
    static const QPoint offsetPoint(-ms_fixedSize.width() / 2, -ms_fixedSize.height() / 2);
    painter->translate(m_pos);
    painter->rotate(m_rotationSprite);//��һ����֮ǰ��draw����û���������ģ�ʵ����ת

    // ���Ƶ���
    painter->drawPixmap(offsetPoint, m_sprite);
    painter->restore();
}


void Enemy::move()
{
    if (!m_active)//û�м���Ĺֲ��ܶ�
        return;

    if (collisionWithCircle(m_pos, 1, m_destinationWayPoint->pos(), 1))
    {
        // ���˵ִ���һ������
        if (m_destinationWayPoint->nextWayPoint())
        {
            // ������һ������
            m_pos = m_destinationWayPoint->pos();
            m_destinationWayPoint = m_destinationWayPoint->nextWayPoint();
        }
        else
        {
            // ��ʾ�������
            m_game->getHpDamage();
            m_game->removedEnemy(this);
            return;
        }
    }

    // ����ǰ�������·��
    // Ŀ�꺽�������
    QPoint targetPoint = m_destinationWayPoint->pos();

    // δ���޸������������ƶ�״̬,�ӿ�,����,m_walkingSpeed�ǻ�׼
    qreal movementSpeed = m_walkingSpeed;
    // ������׼��
    // �ƶ��ٶ���ʵÿ�ζ���1,normalizedȡֵֻ��(1,0)/(-1,0)/(0,1)/(0,-1)����,��Ҫ�������������ת�Ƕ�(0/90/180/270)
    QVector2D normalized(targetPoint - m_pos);
    normalized.normalize();
    m_pos = m_pos + normalized.toPoint() * movementSpeed;

    // ȷ������ѡ����
    // Ĭ��ͼƬ����,��Ҫ����180��ת��
    m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x()));
}

void Enemy::getDamage(int damage)
{
//    m_game->audioPlayer()->playSound(LaserShootSound);
    m_currentHp -= damage;//�ۼ������յ����˺�

    // ����,��Ҫ�Ƴ�
    if (m_currentHp <= 0)
    {
//        m_game->audioPlayer()->playSound(EnemyDestorySound);
        m_game->awardGold(200);//����һ�����˽������
        getRemoved();//������ʧ
    }
}

void Enemy::getRemoved()
{
    if (m_attackedTowersList.empty())
        return;

    foreach (Tower *attacker, m_attackedTowersList)
        attacker->targetKilled();
    // ֪ͨgame,�˵����Ѿ�����
    m_game->removedEnemy(this);
}

void Enemy::getAttacked(Tower *attacker)
{
    m_attackedTowersList.push_back(attacker);
}

// ���������Ѿ������˹�����Χ

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
//��������ۺ���֮��enemy�ſ����ж�

Enemy::~Enemy(){}


