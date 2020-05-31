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

    virtual void draw(QPainter *painter) const;   //已实现
    virtual void checkEnemyInRange();             //检测敌人是否在攻击范围内，已实现
    virtual void targetKilled();                  //击杀敌人，已实现
    virtual void attackEnemy();                   //攻击敌人，已实现
    void chooseEnemyForAttack(Enemy *enemy);      //选定攻击敌人（与正在攻击的敌人连接），已实现
    virtual void lostSightOfEnemy();              //敌人超出攻击范围，已实现
    int getTowerType();                           //不同类型的塔，未实现

public slots:
    virtual void shootWeapon();                   //该槽函数与m_fireRate与m_fireRateTimer关联，管理攻击频率，已实现

protected:
    int             m_towerType=1;                //多种塔类型编号
    int             m_towerLevel;                 //后期升级，未实现

    bool			m_attacking;                  //是否正在攻击，一个塔不能同时攻击两个敌人
    int				m_attackRange;	              //代表塔的攻击半径
    int				m_damage;		              //代表塔对敌人单位时间内的伤害
    int				m_fireRate;		              //代表塔的攻击频率
    qreal			m_rotationSprite;             //实时跟进敌人实现塔的旋转效果

    Enemy *			m_chooseEnemy;                //锁定攻击敌人
    MainWindow *	m_game;                       //一个指针，传回mainwindow界面实现paintevent
    QTimer *		m_fireRateTimer;              //进攻速率计时器

    //常规三件套
    const QPoint	m_pos;
    const QPixmap	m_sprite;

    static const QSize ms_fixedSize;

signals:

};

#endif // TOWER_H
