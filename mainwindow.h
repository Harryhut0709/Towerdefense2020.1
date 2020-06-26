//## 这个类在第二次修改的时候做了很大的变动
//因为新建了一个关卡，所以这个类有较大的的改动。基类Mainwindow现在只具有每个关卡
//共有的成员函数（需要重载的以虚函数实现，并且每个关卡不同的信息：如塔的放置位置、
//怪物信息、航点信息等函数以给空实现，两个关卡分别重载这些函数给出不同的关卡信息；
//共有的判定逻辑和功能则不是虚函数：如鼠标点击事件、怪物到达基地扣血、一处敌人、移除
//子弹、奖励金币、可买塔/升级等函数）和数据成员（如波数、金币、血量信息；攻击塔位、
//攻击塔、航点、敌人、子弹列表等）。

//两个派生类Mainwindow1和Mainwindow2均公有继承于Mainwindow类，关卡信息函数重载，
//放在关卡构造函数中调用

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QPushButton>
#include "towerposition.h"
#include "tower.h"
#include "soundcontrol.h"

namespace Ui {
class MainWindow;
}

class Waypoint;
class Enemy;
class Bullet;
//头文件之间不能相互包含

class MainWindow : public QMainWindow
{
    Q_OBJECT//所有应用QT槽的类都需要声明

public:
    virtual void paintEvent(QPaintEvent *);///
    explicit MainWindow(QWidget *parent = 0);

    void getHpDamage(int damage = 1);                   //敌方进入基地，扣血
    void removedEnemy(Enemy *enemy);                    //敌方被打死，消失
    void removedBullet(Bullet *bullet);                 //子弹击中敌人后，消失(移除)
    void addBullet(Bullet *bullet);                     //装填新的子弹
    void awardGold(int gold);                           //打死一个敌人之后奖励金币

    void mousePressEvent(QMouseEvent *);                //鼠标点击触发事件
    bool canBuyTower() const;                           //判断是否能够买新的塔
    bool canUpgradeTower() const;                       //++ //判断是否能升级
    virtual void drawWave(QPainter *painter);///
    virtual void drawHP(QPainter *painter);///
    virtual void drawPlayerGold(QPainter *painter);///             //这三个函数在顶顶上显示当前信息，随时更新，暂时先以丑陋的文字实现
    virtual void drawOtherTexts(QPainter *painter);///          //+ //其他文字信息

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //游戏中涉及到的怪物以QList形式存储

    ~MainWindow();

    //## 全部修改成了public 用于继承
    virtual void loadTowerPositions();///                          //初始化塔的摆放位置
    virtual void addWayPoints();///                               //初始化怪物的行走路线（航点）
    virtual void loadTowerSelectButtons();///                      //+ //初始化选塔按钮
    virtual void loadEndButtons();                               //++ //结束按钮
    virtual bool loadWave();///                                    //初始化波数信息

    void doGameOver();                                  //游戏结束，播放胜利或者失败动画，未实现

public slots://QT的private槽当前类及其子类可以将信号与之相连接
    void updateMap();                                   //30ms刷新一次，模拟动画帧数

protected:
    Ui::MainWindow *ui;

    int                     m_currentTowerMode=1;       //+ //通过这个变量来确定安放的不同塔

    int						m_waves;                    //波数
    int						m_playerHp;                 //基地血量
    int						m_playerGold;               //玩家金币

    bool                    m_gameStart;               //++游戏是否开始或者暂停
    bool					m_gameEnded;
    bool					m_gameWin;

    SoundControl *          m_soundcontrol;             //+

    QList<TowerPosition>    m_towerPositionsList;
    QList<Tower *>          m_towersList;
    QList<Waypoint *>       m_waypointsList;
    QList<Enemy *>			m_enemyList;
    QList<Bullet *>			m_bulletList;

};

class MainWindow1:public MainWindow//第一关
{
    Q_OBJECT//所有应用QT槽的类都需要声明

public:
    virtual void paintEvent(QPaintEvent *);
    explicit MainWindow1(QWidget *parent = 0);

    void drawWave(QPainter *painter);
    void drawHP(QPainter *painter);
    void drawPlayerGold(QPainter *painter);             //这三个函数在顶顶上显示当前信息，随时更新，暂时先以丑陋的文字实现
    void drawOtherTexts(QPainter *painter);             //+ //其他文字信息
    //位置改变需要重载

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //游戏中涉及到的怪物以QList形式存储

    ~MainWindow1();

    //# 全部修改成了public 用于继承
    void loadTowerPositions();                          //初始化塔的摆放位置
    void addWayPoints();                                //初始化怪物的行走路线（航点）
    void loadTowerSelectButtons();                      //+ //初始化选塔按钮
    void loadEndButtons();                              //++ //结束按钮
    bool loadWave();                                    //初始化波数信息
};

class MainWindow2:public MainWindow//第二关
{
    Q_OBJECT//所有应用QT槽的类都需要声明

public:
    virtual void paintEvent(QPaintEvent *);
    explicit MainWindow2(QWidget *parent = 0);

    void drawWave(QPainter *painter);
    void drawHP(QPainter *painter);
    void drawPlayerGold(QPainter *painter);             //这三个函数在顶顶上显示当前信息，随时更新，暂时先以丑陋的文字实现
    void drawOtherTexts(QPainter *painter);             //+ //其他文字信息
    //位置改变需要重载

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //游戏中涉及到的怪物以QList形式存储

    ~MainWindow2();

    //# 全部修改成了public 用于继承
    void loadTowerPositions();                          //初始化塔的摆放位置
    void addWayPoints();                                //初始化怪物的行走路线（航点）
    void addWayPoints1();                               //初始化第二条路线 //++
    void loadTowerSelectButtons();                      //+ //初始化选塔按钮
    void loadEndButtons();                              //++ //结束按钮
    bool loadWave();                                    //初始化波数信息

private:
    QList<Waypoint *>       m_waypointsList1;             //第二条路线
};

#endif // MAINWINDOW_H
