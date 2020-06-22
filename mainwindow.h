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
//ͷ�ļ�֮�䲻���໥����

class MainWindow : public QMainWindow
{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����

public:
    void paintEvent(QPaintEvent *);
    explicit MainWindow(QWidget *parent = 0);

//    void startGame();                                   //+ //��Ϸ��ʼ������m_gameStart��ֵ

    void getHpDamage(int damage = 1);                   //�з�������أ���Ѫ
    void removedEnemy(Enemy *enemy);                    //�з�����������ʧ
    void removedBullet(Bullet *bullet);                 //�ӵ����е��˺���ʧ(�Ƴ�)
    void addBullet(Bullet *bullet);                     //װ���µ��ӵ�
    void awardGold(int gold);                           //����һ������֮�������

    void mousePressEvent(QMouseEvent *);                //����������¼�
    bool canBuyTower() const;                           //�ж��Ƿ��ܹ����µ���
    bool canUpgradeTower() const;                       //++ //�ж��Ƿ�������
    void drawWave(QPainter *painter);
    void drawHP(QPainter *painter);
    void drawPlayerGold(QPainter *painter);             //�����������ڶ�������ʾ��ǰ��Ϣ����ʱ���£���ʱ���Գ�ª������ʵ��
    void drawOtherTexts(QPainter *painter);             //+ //����������Ϣ

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //��Ϸ���漰���Ĺ�����QList��ʽ�洢

    ~MainWindow();

private:
    void loadTowerPositions();                          //��ʼ�����İڷ�λ��
    void addWayPoints();                                //��ʼ�����������·�ߣ����㣩
    void loadTowerSelectButtons();                      //+ //��ʼ��ѡ����ť
    bool loadWave();                                    //��ʼ��������Ϣ

    void doGameOver();                                  //��Ϸ����������ʤ������ʧ�ܶ�����δʵ��

private slots://QT��private�۵�ǰ�༰��������Խ��ź���֮������
    void updateMap();                                   //30msˢ��һ�Σ�ģ�⶯��֡��

private:
    Ui::MainWindow *ui;

    int                     m_currentTowerMode=1;       //+ //ͨ�����������ȷ�����ŵĲ�ͬ��

    int						m_waves;                    //����
    int						m_playerHp;                 //����Ѫ��
    int						m_playerGold;               //��ҽ��

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
