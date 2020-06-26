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
class Button;

class Tower : public QObject
{
    Q_OBJECT
public:
    explicit Tower(QObject *parent = nullptr);
    Tower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower1.png"));
    ~Tower();

    virtual void draw(QPainter *painter) const;   //��ʵ�� //+  //��ΪͼƬ��Դ��ԭ�������������ĵ㲻һ��������draw()ҲҪ����
    virtual void checkEnemyInRange();             //�������Ƿ��ڹ�����Χ�ڣ���ʵ��
    virtual void targetKilled();                  //��ɱ���ˣ���ʵ��
    virtual void attackEnemy();                   //�������ˣ���ʵ��
    virtual void chooseEnemyForAttack(Enemy *enemy);      //ѡ���������ˣ������ڹ����ĵ������ӣ�����ʵ��
    virtual void lostSightOfEnemy();              //���˳���������Χ����ʵ��
    int getTowerType();                           //��ͬ���͵�����//+ ��ʵ��
    const QPoint gettowerpos() const;                   //++ //������ʱ����Ҫ�ж����ĵ��Ƿ��غ�

    //+ //�����Ͳ��
    virtual void setTowerLevel(int level);                      //++ �������Ҹ�����������
    virtual bool isMaxlevel() const;                            //�ô������Ƿ�������������������
    const QPoint upgradebuttoncenterPos() const;                //��������ť�����ĵ�
    const QPoint sellbuttoncenterPos() const;                   //������ť�����ĵ�
    virtual bool containupgradebutton(const QPoint &pos) const; //�Ƿ�㵽��������ť��
    virtual bool containsellbutton(const QPoint &pos) const;    //�Ƿ�㵽��������ť��
    virtual void disconnectEnemyForAttack(Tower *attacker);     //++ //������ȡ������˵Ĺ���


public slots:
    virtual void shootWeapon();                   //�òۺ�����m_fireRate��m_fireRateTimer������������Ƶ�ʣ���ʵ��

protected:
    int             m_towerType=1;                //���������ͱ��(Ĭ��Ϊ1)
    int             m_towerLevel=1;                 //����������δʵ��

    bool			m_attacking;                  //�Ƿ����ڹ�����һ��������ͬʱ������������
    int				m_attackRange;	              //�������Ĺ����뾶
    int				m_damage;		              //�������Ե��˵�λʱ���ڵ��˺�
    int				m_fireRate;		              //�������Ĺ���Ƶ��
    qreal			m_rotationSprite;             //ʵʱ��������ʵ��������תЧ��

    Enemy *			m_chooseEnemy;                //������������
    MainWindow *	m_game;                       //һ��ָ�룬����mainwindow����ʵ��paintevent
    QTimer *		m_fireRateTimer;              //�������ʼ�ʱ��

    //+
//����һ����button�����󶨣����ֲ���ʵ��
//    Button *        m_upgradebutton;              //������ť
//    Button *        m_sellbutton;                 //������ť

//������������ť��ͼƬ�����󶨣����mousepresseventʵ��
    const QPixmap   m_upgradebutton;
    const QPixmap   m_sellbutton;

    //����������
    const QPoint	m_pos;
    const QPixmap	m_sprite;

    static const QSize ms_fixedSize;

    //+
    static const QSize ms_buttonfixedSize;//��ť�ĳߴ�

signals:

};

//+
class IceTower:public Tower{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����
public:
    IceTower(QPoint pos, MainWindow *game,const QPixmap &sprite = QPixmap(":/image/tower2.png"));
    ~IceTower();
    void draw(QPainter *painter) const;//��ΪͼƬ��Դ��ԭ�������������ĵ㲻һ��������draw()ҲҪ����
//    void chooseEnemyForAttack(Enemy *enemy);//���еĵ��˼��٣�������Ҫ���� //��һ����icebullet��ʵ��,��Ϊbullet�뱻�����ĵ���ֱ������
    void setTowerLevel(int level);//++ //�������Բ�ͬ��������Ҫ����

public slots:
    void shootWeapon();//�ӵ��仯��������Ҫ����

protected:
    int             m_towerType=2;
};

class PoisonTower:public Tower{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����
public:
    PoisonTower(QPoint pos, MainWindow *game,const QPixmap &sprite = QPixmap(":/image/tower3.png"));
    ~PoisonTower();
    void draw(QPainter *painter) const;//��ΪͼƬ��Դ��ԭ�������������ĵ㲻һ��������draw()ҲҪ����
//    void chooseEnemyForAttack(Enemy *enemy);//���еĵ����ж���������Ҫ����
    void setTowerLevel(int level);//++ //�������Բ�ͬ��������Ҫ����

public slots:
    void shootWeapon();//�ӵ��仯��������Ҫ����

protected:
    int             m_towerType=3;
};

#endif // TOWER_H
