#include "enemy.h"
#include "utility.h"
#include <QTimer> //+ //���ڿ��Ʊ���ʱ��
#include <QObject> //+ //����connect����

static const int Health_Bar_Width = 40;

const QSize Enemy::ms_fixedSize(110, 42);

Enemy::Enemy(QObject *parent) : QObject(parent){}

Enemy::Enemy(Waypoint *startWayPoint, MainWindow *game,int level /*= 1 */,
             const QPixmap &sprite /* = QPixmap(":/image/enemy1.png") */,
             int maxHp /* = 40 */,qreal walkingSpeed /*= 2.0 */)
    : QObject(0)
    , m_active(false)             //ע�������ʼ��
    , m_isSlowed(false)
    , m_isPoisoned(false)         //+ //��ʼʱ��false,������ӵ����к�һ��ʱ���ܵ���Ӧ�˺�
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
    // +
    if(m_isSlowed)//��icebullet����,����Ϊԭ����һ�룬����ʱ������޸�
    {
        //����һ��timer���Ʊ���ʱ��
        QTimer *frozentimer = new QTimer();
        frozentimer->setInterval(2000);//��������ʱ��
        connect(frozentimer,SIGNAL(timeout()),this,SLOT(SpeedUp()));
        frozentimer->start();
        qreal movementSpeed = 0.3 * m_walkingSpeed;
        // ������׼��
        // �ƶ��ٶ���ʵÿ�ζ���1,normalizedȡֵֻ��(1,0)/(-1,0)/(0,1)/(0,-1)����,��Ҫ�������������ת�Ƕ�(0/90/180/270)
        QVector2D normalized(targetPoint - m_pos);
        normalized.normalize();
        m_pos = m_pos + normalized.toPoint() * movementSpeed;
        m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x()));
        // ȷ������ѡ����
        // Ĭ��ͼƬ����,��Ҫ����180��ת��
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
    m_currentHp -= damage;//�ۼ������յ����˺�

    if (m_isPoisoned)//+
    {
        QTimer *stoptimer = new QTimer();//����������ƽ���ʱ��
        QTimer *poisontimer = new QTimer();
        poisontimer->setInterval(1000);//ÿ���ж��˺�ʱ��
        stoptimer->setInterval(5000);//�����ж�ʱ��
        stoptimer->setSingleShot(true);//ֻ����һ��
        connect(poisontimer,SIGNAL(timeout()),this,SLOT(getPoisonDamage()));
        connect(stoptimer,SIGNAL(timeout()),this,SLOT(CurePoison()));
        poisontimer->start();
        stoptimer->start();
    }

    // ����,��Ҫ�Ƴ�
    if (m_currentHp <= 0)
    {
        m_game->awardGold(200);//����һ�����˽������
        getRemoved();//������ʧ
    }
}

void Enemy::getRemoved()//++ //�������ж�Ч�����˴���Ҫ�޸��߼� //����û���ҵ���ȷ���߼�����ж�bug...
{
    if (m_attackedTowersList.empty() && m_currentHp>0 )
        return;

    if (m_currentHp<=0 && m_isPoisoned==1)
    {
//        foreach (Tower *attacker, m_attackedTowersList)
//            attacker->targetKilled();
//        // ֪ͨgame,�˵����Ѿ�����
        m_game->removedEnemy(this);
    }

    else if (m_currentHp<=0 && m_isPoisoned==0)
    {
                foreach (Tower *attacker, m_attackedTowersList)
                    attacker->targetKilled();
                // ֪ͨgame,�˵����Ѿ�����
                m_game->removedEnemy(this);
    }
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

int Enemy::getCurrentHp() const
{
    return m_currentHp;
}

void Enemy::slowDown()//+ //������ //״̬ת��
{
    m_isSlowed=true;
}

void Enemy::SpeedUp()//+ //������� //������ǲۺ�����������
{
    m_isSlowed=false;
}

void Enemy::getPoisoned()//+ //�ж�
{
    m_isPoisoned=true;
}

void Enemy::CurePoison()//+ //�ж���� //�ۺ���
{
    m_isPoisoned=false;
}

void Enemy::getPoisonDamage()//+ //�ж��˺� //�ۺ���
{
    m_currentHp -= 10;
}

void Enemy::doActivate()
{
    m_active = true;
}
//��������ۺ���֮��enemy�ſ����ж�

Enemy::~Enemy(){}
