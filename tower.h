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

    virtual void draw(QPainter *painter) const;   //已实现 //+  //因为图片资源的原因，三个塔的中心点不一样，所以draw()也要重载
    virtual void checkEnemyInRange();             //检测敌人是否在攻击范围内，已实现
    virtual void targetKilled();                  //击杀敌人，已实现
    virtual void attackEnemy();                   //攻击敌人，已实现
    virtual void chooseEnemyForAttack(Enemy *enemy);      //选定攻击敌人（与正在攻击的敌人连接），已实现
    virtual void lostSightOfEnemy();              //敌人超出攻击范围，已实现
    int getTowerType();                           //不同类型的塔，//+ 已实现
    const QPoint gettowerpos() const;                   //++ //拆塔的时候需要判断中心点是否重合

    //+ //升级和拆除
    virtual void setTowerLevel(int level);                      //++ 升级并且更新能力参数
    virtual bool isMaxlevel() const;                            //该处的塔是否满级，还可以升级否
    const QPoint upgradebuttoncenterPos() const;                //升级塔按钮的中心点
    const QPoint sellbuttoncenterPos() const;                   //卖塔按钮的中心点
    virtual bool containupgradebutton(const QPoint &pos) const; //是否点到了升级按钮上
    virtual bool containsellbutton(const QPoint &pos) const;    //是否点到了卖塔按钮上
    virtual void disconnectEnemyForAttack(Tower *attacker);     //++ //拆塔后取消与敌人的关联


public slots:
    virtual void shootWeapon();                   //该槽函数与m_fireRate与m_fireRateTimer关联，管理攻击频率，已实现

protected:
    int             m_towerType=1;                //多种塔类型编号(默认为1)
    int             m_towerLevel=1;                 //后期升级，未实现

    bool			m_attacking;                  //是否正在攻击，一个塔不能同时攻击两个敌人
    int				m_attackRange;	              //代表塔的攻击半径
    int				m_damage;		              //代表塔对敌人单位时间内的伤害
    int				m_fireRate;		              //代表塔的攻击频率
    qreal			m_rotationSprite;             //实时跟进敌人实现塔的旋转效果

    Enemy *			m_chooseEnemy;                //锁定攻击敌人
    MainWindow *	m_game;                       //一个指针，传回mainwindow界面实现paintevent
    QTimer *		m_fireRateTimer;              //进攻速率计时器

    //+
//方法一：加button与塔绑定，发现不能实现
//    Button *        m_upgradebutton;              //升级按钮
//    Button *        m_sellbutton;                 //拆塔按钮

//方法二：将按钮的图片与塔绑定，结合mousepressevent实现
    const QPixmap   m_upgradebutton;
    const QPixmap   m_sellbutton;

    //常规三件套
    const QPoint	m_pos;
    const QPixmap	m_sprite;

    static const QSize ms_fixedSize;

    //+
    static const QSize ms_buttonfixedSize;//按钮的尺寸

signals:

};

//+
class IceTower:public Tower{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    IceTower(QPoint pos, MainWindow *game,const QPixmap &sprite = QPixmap(":/image/tower2.png"));
    ~IceTower();
    void draw(QPainter *painter) const;//因为图片资源的原因，三个塔的中心点不一样，所以draw()也要重载
//    void chooseEnemyForAttack(Enemy *enemy);//击中的敌人减速，所以需要重载 //这一步在icebullet中实现,因为bullet与被攻击的敌人直接相连
    void setTowerLevel(int level);//++ //塔的属性不同，所以需要重载

public slots:
    void shootWeapon();//子弹变化，所以需要重载

protected:
    int             m_towerType=2;
};

class PoisonTower:public Tower{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    PoisonTower(QPoint pos, MainWindow *game,const QPixmap &sprite = QPixmap(":/image/tower3.png"));
    ~PoisonTower();
    void draw(QPainter *painter) const;//因为图片资源的原因，三个塔的中心点不一样，所以draw()也要重载
//    void chooseEnemyForAttack(Enemy *enemy);//击中的敌人中毒，所以需要重载
    void setTowerLevel(int level);//++ //塔的属性不同，所以需要重载

public slots:
    void shootWeapon();//子弹变化，所以需要重载

protected:
    int             m_towerType=3;
};

#endif // TOWER_H
