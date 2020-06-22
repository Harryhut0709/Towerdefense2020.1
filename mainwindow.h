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
    void paintEvent(QPaintEvent *);
    explicit MainWindow(QWidget *parent = 0);

//    void startGame();                                   //+ //游戏开始，更改m_gameStart的值

    void getHpDamage(int damage = 1);                   //敌方进入基地，扣血
    void removedEnemy(Enemy *enemy);                    //敌方被打死，消失
    void removedBullet(Bullet *bullet);                 //子弹击中敌人后，消失(移除)
    void addBullet(Bullet *bullet);                     //装填新的子弹
    void awardGold(int gold);                           //打死一个敌人之后奖励金币

    void mousePressEvent(QMouseEvent *);                //鼠标点击触发事件
    bool canBuyTower() const;                           //判断是否能够买新的塔
    bool canUpgradeTower() const;                       //++ //判断是否能升级
    void drawWave(QPainter *painter);
    void drawHP(QPainter *painter);
    void drawPlayerGold(QPainter *painter);             //这三个函数在顶顶上显示当前信息，随时更新，暂时先以丑陋的文字实现
    void drawOtherTexts(QPainter *painter);             //+ //其他文字信息

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //游戏中涉及到的怪物以QList形式存储

    ~MainWindow();

private:
    void loadTowerPositions();                          //初始化塔的摆放位置
    void addWayPoints();                                //初始化怪物的行走路线（航点）
    void loadTowerSelectButtons();                      //+ //初始化选塔按钮
    bool loadWave();                                    //初始化波数信息

    void doGameOver();                                  //游戏结束，播放胜利或者失败动画，未实现

private slots://QT的private槽当前类及其子类可以将信号与之相连接
    void updateMap();                                   //30ms刷新一次，模拟动画帧数

private:
    Ui::MainWindow *ui;

    int                     m_currentTowerMode=1;       //+ //通过这个变量来确定安放的不同塔

    int						m_waves;                    //波数
    int						m_playerHp;                 //基地血量
    int						m_playerGold;               //玩家金币

    bool					m_gameEnded;
    bool					m_gameWin_first;

    SoundControl *          m_soundcontrol;             //+

    QList<TowerPosition>    m_towerPositionsList;
    QList<Tower *>          m_towersList;
    QList<Waypoint *>       m_waypointsList;
    QList<Enemy *>			m_enemyList;
    QList<Bullet *>			m_bulletList;

};

#endif // MAINWINDOW_H
