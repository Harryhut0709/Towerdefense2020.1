//������ǶԵз����޵�����
#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QPainter>

#include "mainwindow.h"
#include "waypoint.h"
#include "tower.h"

class Enemy : public QObject
{
    Q_OBJECT
public:
    explicit Enemy(QObject *parent = nullptr);
    Enemy(Waypoint *startWayPoint, MainWindow *game,int level = 1,
          const QPixmap &sprite = QPixmap(":/image/enemy1.png"),
          int maxHp = 40,qreal walkingSpeed = 2.0);
    ~Enemy();
    //����Ϊ�麯��
    virtual void draw(QPainter *painter) const;     //������
    virtual void move();                            //���޸��ݺ���˳���ƶ�
    virtual void getDamage(int damage);             //�ܵ��˺�
    virtual void getRemoved();                      //�����������ʧ
    virtual void getAttacked(Tower *attacker);      //�������Ĺ�����Χ��������
    virtual void gotLostSight(Tower *attacker);     //�뿪���Ĺ�����Χ���������
    virtual void setLevel(int level);               //���ù��޵ȼ�
    virtual int  getCurrentHp() const;              //+ //�ж��Ǳ������˻����ߵ������ˣ�������

    //δʵ��
    void slowDown();                                //��icebullet���У��޸�m_isSlowed��ֵΪ1
    void getPoisoned();                             //��poisonbullet���У�����һ��ʱ��ĳ����˺�
    //+

    QPoint pos() const;                             //���ع��޵�ǰλ��

public slots:
    void doActivate();                              //�ۺ��������ƹ�����waypoints�ƶ�
    void SpeedUp();                                 //�ۺ������޸�m_isSlowed��ֵΪ0 //+
    void CurePoison();                              //�ۺ������޸�m_isPoisoned��ֵΪ1 //+
    void getPoisonDamage();                         //�ۺ������ж������˺� //+

protected://Ϊ�̳���׼��
    bool			m_active;                 //��ʾ�Ƿ�����ƶ���ֻ�е���Ϊtrueʱ�����˲ſ����ƶ�
    bool            m_isSlowed=0;             //��icebullet���У�����һ��ʱ��ļ��� //Ĭ��Ϊ0�������к�һ��ʱ���ֵΪ1����ɱ�������
    bool            m_isPoisoned=0;           //��poisonbullet���У�����һ��ʱ��ĳ����˺� //Ĭ��Ϊ0��������һ��ʱ����ֵΪ1����ɳ����ж��˺�

    int				m_maxHp;                  //���Ѫ��
    int				m_currentHp;              //��ǰѪ��
    int             m_level;                  //���˵ȼ�
    qreal			m_walkingSpeed;           //qreal�����൱��double
    qreal			m_rotationSprite;         //�洢���˵���һ������ʱͼƬ����ת�Ƕ�

    Waypoint *		m_destinationWayPoint;    //�����洢��ǰ����
    MainWindow *	m_game;                   //����MainWindowʵ��paintevent
    QList<Tower *>	m_attackedTowersList;     //�����Ŀ�����������ڹ�����������Ϊ����ͬʱ�ж�����ڴ�һ�����ˣ�������Ҫenemy��ά����������QList��

    //����������
    QPoint			m_pos;
    const QPixmap	m_sprite;
    static const QSize ms_fixedSize;
signals:

};

#endif // ENEMY_H
