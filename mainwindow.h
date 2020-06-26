//## ������ڵڶ����޸ĵ�ʱ�����˺ܴ�ı䶯
//��Ϊ�½���һ���ؿ�������������нϴ�ĵĸĶ�������Mainwindow����ֻ����ÿ���ؿ�
//���еĳ�Ա��������Ҫ���ص����麯��ʵ�֣�����ÿ���ؿ���ͬ����Ϣ�������ķ���λ�á�
//������Ϣ��������Ϣ�Ⱥ����Ը���ʵ�֣������ؿ��ֱ�������Щ����������ͬ�Ĺؿ���Ϣ��
//���е��ж��߼��͹��������麯������������¼������ﵽ����ؿ�Ѫ��һ�����ˡ��Ƴ�
//�ӵ���������ҡ�������/�����Ⱥ����������ݳ�Ա���粨������ҡ�Ѫ����Ϣ��������λ��
//�����������㡢���ˡ��ӵ��б�ȣ���

//����������Mainwindow1��Mainwindow2�����м̳���Mainwindow�࣬�ؿ���Ϣ�������أ�
//���ڹؿ����캯���е���

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
    virtual void paintEvent(QPaintEvent *);///
    explicit MainWindow(QWidget *parent = 0);

    void getHpDamage(int damage = 1);                   //�з�������أ���Ѫ
    void removedEnemy(Enemy *enemy);                    //�з�����������ʧ
    void removedBullet(Bullet *bullet);                 //�ӵ����е��˺���ʧ(�Ƴ�)
    void addBullet(Bullet *bullet);                     //װ���µ��ӵ�
    void awardGold(int gold);                           //����һ������֮�������

    void mousePressEvent(QMouseEvent *);                //����������¼�
    bool canBuyTower() const;                           //�ж��Ƿ��ܹ����µ���
    bool canUpgradeTower() const;                       //++ //�ж��Ƿ�������
    virtual void drawWave(QPainter *painter);///
    virtual void drawHP(QPainter *painter);///
    virtual void drawPlayerGold(QPainter *painter);///             //�����������ڶ�������ʾ��ǰ��Ϣ����ʱ���£���ʱ���Գ�ª������ʵ��
    virtual void drawOtherTexts(QPainter *painter);///          //+ //����������Ϣ

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //��Ϸ���漰���Ĺ�����QList��ʽ�洢

    ~MainWindow();

    //## ȫ���޸ĳ���public ���ڼ̳�
    virtual void loadTowerPositions();///                          //��ʼ�����İڷ�λ��
    virtual void addWayPoints();///                               //��ʼ�����������·�ߣ����㣩
    virtual void loadTowerSelectButtons();///                      //+ //��ʼ��ѡ����ť
    virtual void loadEndButtons();                               //++ //������ť
    virtual bool loadWave();///                                    //��ʼ��������Ϣ

    void doGameOver();                                  //��Ϸ����������ʤ������ʧ�ܶ�����δʵ��

public slots://QT��private�۵�ǰ�༰��������Խ��ź���֮������
    void updateMap();                                   //30msˢ��һ�Σ�ģ�⶯��֡��

protected:
    Ui::MainWindow *ui;

    int                     m_currentTowerMode=1;       //+ //ͨ�����������ȷ�����ŵĲ�ͬ��

    int						m_waves;                    //����
    int						m_playerHp;                 //����Ѫ��
    int						m_playerGold;               //��ҽ��

    bool                    m_gameStart;               //++��Ϸ�Ƿ�ʼ������ͣ
    bool					m_gameEnded;
    bool					m_gameWin;

    SoundControl *          m_soundcontrol;             //+

    QList<TowerPosition>    m_towerPositionsList;
    QList<Tower *>          m_towersList;
    QList<Waypoint *>       m_waypointsList;
    QList<Enemy *>			m_enemyList;
    QList<Bullet *>			m_bulletList;

};

class MainWindow1:public MainWindow//��һ��
{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����

public:
    virtual void paintEvent(QPaintEvent *);
    explicit MainWindow1(QWidget *parent = 0);

    void drawWave(QPainter *painter);
    void drawHP(QPainter *painter);
    void drawPlayerGold(QPainter *painter);             //�����������ڶ�������ʾ��ǰ��Ϣ����ʱ���£���ʱ���Գ�ª������ʵ��
    void drawOtherTexts(QPainter *painter);             //+ //����������Ϣ
    //λ�øı���Ҫ����

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //��Ϸ���漰���Ĺ�����QList��ʽ�洢

    ~MainWindow1();

    //# ȫ���޸ĳ���public ���ڼ̳�
    void loadTowerPositions();                          //��ʼ�����İڷ�λ��
    void addWayPoints();                                //��ʼ�����������·�ߣ����㣩
    void loadTowerSelectButtons();                      //+ //��ʼ��ѡ����ť
    void loadEndButtons();                              //++ //������ť
    bool loadWave();                                    //��ʼ��������Ϣ
};

class MainWindow2:public MainWindow//�ڶ���
{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����

public:
    virtual void paintEvent(QPaintEvent *);
    explicit MainWindow2(QWidget *parent = 0);

    void drawWave(QPainter *painter);
    void drawHP(QPainter *painter);
    void drawPlayerGold(QPainter *painter);             //�����������ڶ�������ʾ��ǰ��Ϣ����ʱ���£���ʱ���Գ�ª������ʵ��
    void drawOtherTexts(QPainter *painter);             //+ //����������Ϣ
    //λ�øı���Ҫ����

    SoundControl * soundcontrol() const;                //+
    QList<Enemy *> enemyList() const;                   //��Ϸ���漰���Ĺ�����QList��ʽ�洢

    ~MainWindow2();

    //# ȫ���޸ĳ���public ���ڼ̳�
    void loadTowerPositions();                          //��ʼ�����İڷ�λ��
    void addWayPoints();                                //��ʼ�����������·�ߣ����㣩
    void addWayPoints1();                               //��ʼ���ڶ���·�� //++
    void loadTowerSelectButtons();                      //+ //��ʼ��ѡ����ť
    void loadEndButtons();                              //++ //������ť
    bool loadWave();                                    //��ʼ��������Ϣ

private:
    QList<Waypoint *>       m_waypointsList1;             //�ڶ���·��
};

#endif // MAINWINDOW_H
