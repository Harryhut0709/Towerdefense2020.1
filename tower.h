#ifndef TOWER_H
#define TOWER_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>
#include <QPainter>

class QPainter;
class Enemy;
class MainWindow;
class QTimer;


class Tower : public QObject
{
    Q_OBJECT
public:
    explicit Tower(QObject *parent = nullptr);
    Tower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower1.png"));
    ~Tower();

    virtual void draw(QPainter *painter) const;   //��ʵ��
    virtual void checkEnemyInRange();             //�������Ƿ��ڹ�����Χ�ڣ���ʵ��
    virtual void targetKilled();                  //��ɱ���ˣ���ʵ��
    virtual void attackEnemy();                   //�������ˣ���ʵ��
    void chooseEnemyForAttack(Enemy *enemy);      //ѡ���������ˣ������ڹ����ĵ������ӣ�����ʵ��
    virtual void lostSightOfEnemy();              //���˳���������Χ����ʵ��
    int getTowerType();                           //��ͬ���͵�����δʵ��

public slots:
    virtual void shootWeapon();                   //�òۺ�����m_fireRate��m_fireRateTimer������������Ƶ�ʣ���ʵ��

protected:
    int             m_towerType=1;                //���������ͱ��
    int             m_towerLevel;                 //����������δʵ��

    bool			m_attacking;                  //�Ƿ����ڹ�����һ��������ͬʱ������������
    int				m_attackRange;	              //�������Ĺ����뾶
    int				m_damage;		              //�������Ե��˵�λʱ���ڵ��˺�
    int				m_fireRate;		              //�������Ĺ���Ƶ��
    qreal			m_rotationSprite;             //ʵʱ��������ʵ��������תЧ��

    Enemy *			m_chooseEnemy;                //������������
    MainWindow *	m_game;                       //һ��ָ�룬����mainwindow����ʵ��paintevent
    QTimer *		m_fireRateTimer;              //�������ʼ�ʱ��

    //����������
    const QPoint	m_pos;
    const QPixmap	m_sprite;

    static const QSize ms_fixedSize;

signals:

};

#endif // TOWER_H
