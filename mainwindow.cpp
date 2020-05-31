#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
//#include "audioplayer.h"
//#include "plistreader.h"
#include <QPainter>
#include <iostream>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>//ʱ��
#include <QXmlStreamReader>
#include <QtDebug>

using namespace std;

static const int TowerCost = 300;//�趨ÿ����һ����������300���

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_waves(0)
    , m_playerHp(5)//�������������
    , m_playerGold(2000)//�ȿ���
    , m_gameEnded(false)
    , m_gameWin_first(false)
{
    ui->setupUi(this);
    addWayPoints();
    loadTowerPositions();
    loadWave();

    //һ��ѭ���ṹ��ʱ��춨��30ms��һ�Σ��ƶ����ˣ�ģ��֡��
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //�Ż���Ϊ����������ӻ���
    QPixmap cachePix(":/image/setting1.jpg");
    QPainter cachePainter(&cachePix);//��ȫ�����Ƶ�cachePix��һ��ͼ��

    foreach (const TowerPosition &towerPos, m_towerPositionsList)
        towerPos.draw(& cachePainter);//�����ĳ�ʼλ��
    foreach (const Tower *tower, m_towersList)
        tower->draw(& cachePainter);//����
    foreach (const Waypoint *waypoint, m_waypointsList)
        waypoint->draw(& cachePainter);//��·��
    foreach (const Enemy *enemy, m_enemyList)
        enemy->draw(& cachePainter);//������
    foreach (const Bullet *bullet, m_bulletList)
        bullet->draw(&cachePainter);//���ӵ�

    drawWave(&cachePainter);
    drawHP(&cachePainter);
    drawPlayerGold(&cachePainter);//������ʾ������Ϣ

    QPainter painter(this);
        painter.drawPixmap(0, 0, cachePix);//�ٰ�cachePixһ���Ի�����

    if (m_gameEnded || m_gameWin_first)
    {
        QString text = m_gameEnded? "YOU LOST!!!" : "YOU WIN!!!";
        QPainter painter(this);
        painter.setPen(QPen(Qt::red));
        painter.drawText(rect(),Qt::AlignCenter,text);
        return;
    }
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
    QPoint pressPos = event->pos();
    auto it = m_towerPositionsList.begin();
    while (it != m_towerPositionsList.end())
    {
        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower())
            //��Ǯ && �������а���λ�ã����ڰ���λ���� && ����λ���ǿյ�
        {
            m_playerGold -= TowerCost;//��Ǯ
            it->setHasTower();//ռ��
            Tower *tower = new Tower (it->centerPos(),this);
            m_towersList.push_back(tower);
            update();
            break;
        }

        ++it;
    }
}

void MainWindow::getHpDamage(int damage /* =1 */)
{
    m_playerHp -= damage;
    if (m_playerHp<=0)
        doGameOver();
}

void MainWindow::removedEnemy(Enemy *enemy)// �з�����������ʧ
{
    Q_ASSERT(enemy);

    m_enemyList.removeOne(enemy);
    delete enemy;

    // û�й���ͼ�һ��
    if (m_enemyList.empty())
    {
        ++m_waves;// ��ǰ����+1
        // ������ȡ��һ��
        if (!loadWave())
        {
            // ��û����һ��ʱ�������ʾ��Ϸʤ��
            // ������Ϸʤ����־Ϊtrue
            m_gameWin_first = true;
            // ��Ϸʤ��ת����Ϸʤ������������ֻʵ���˵�һ�أ�
            // û��ʤ��Ч��
        }
    }
}

void MainWindow::removedBullet(Bullet *bullet)//�ӵ����е��ˣ���ʧ
{
    Q_ASSERT(bullet);

    m_bulletList.removeOne(bullet);
    delete bullet;
}

void MainWindow::addBullet(Bullet *bullet)
{
    Q_ASSERT(bullet);

    m_bulletList.push_back(bullet);
}

void MainWindow::awardGold(int gold)
{
    m_playerGold += gold;
    update();
}

bool MainWindow::canBuyTower() const
{
    if (m_playerGold >= TowerCost)
    {
        return true;
    }
    else return false;
}

void MainWindow::drawWave(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(400, 5, 100, 25), QString("WAVE : %1").arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(30, 5, 100, 25), QString("HP : %1").arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(200, 5, 200, 25), QString("GOLD : %1").arg(m_playerGold));
}

void MainWindow::loadTowerPositions()
{
    QPoint pos[] =
    {
        QPoint(260,210),
        QPoint(350,210),
        QPoint(260,270),
        QPoint(350,270),
        QPoint(260,330),
        QPoint(350,330),

        QPoint(535,150),
        QPoint(535,210),
        QPoint(535,270),
        QPoint(535,330),
        QPoint(535,390),
        QPoint(625,270),
        QPoint(625,330),
        QPoint(625,390),

        QPoint(625,540),
    };//�������ĳ�ʼλ�ù̶����ʲ���new����
    int len = sizeof(pos) / sizeof(pos[0]);

    for (int i=0;i<len;i++)
    {
        m_towerPositionsList.push_back(pos[i]);
    }
}

void MainWindow::addWayPoints()
{
    Waypoint *waypoint1 = new Waypoint(QPoint(855,210));
    m_waypointsList.push_back(waypoint1);

    Waypoint *waypoint2 = new Waypoint(QPoint(855,290));
    m_waypointsList.push_back(waypoint2);
    waypoint2->setNextWayPoint(waypoint1);

    Waypoint *waypoint3 = new Waypoint(QPoint(770,290));
    m_waypointsList.push_back(waypoint3);
    waypoint3->setNextWayPoint(waypoint2);

    Waypoint *waypoint4 = new Waypoint(QPoint(770,485));
    m_waypointsList.push_back(waypoint4);
    waypoint4->setNextWayPoint(waypoint3);

    Waypoint *waypoint5 = new Waypoint(QPoint(490,485));
    m_waypointsList.push_back(waypoint5);
    waypoint5->setNextWayPoint(waypoint4);

    Waypoint *waypoint6 = new Waypoint(QPoint(490,160));
    m_waypointsList.push_back(waypoint6);
    waypoint6->setNextWayPoint(waypoint5);

    Waypoint *waypoint7 = new Waypoint(QPoint(220,160));
    m_waypointsList.push_back(waypoint7);
    waypoint7->setNextWayPoint(waypoint6);

    Waypoint *waypoint8 = new Waypoint(QPoint(220,350));
    m_waypointsList.push_back(waypoint8);
    waypoint8->setNextWayPoint(waypoint7);

    Waypoint *waypoint9 = new Waypoint(QPoint(20,350));
    m_waypointsList.push_back(waypoint9);
    waypoint9->setNextWayPoint(waypoint8);
}

bool MainWindow::loadWave()
{
    if (m_waves>=6)
    {
        return false;
    }
    Waypoint *startWaypoint = m_waypointsList.back();//֮ǰ������ʵ�֣�β�����ǿ�ʼ�ڵ�
    int enemyStartInterval[] = {1000,2500,4000,5500,7000,8500,10000,11500};
    for (int i=0; i<8; ++i)
    {
        Enemy *enemy = new Enemy(startWaypoint,this);
        m_enemyList.push_back(enemy);
        QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
    }
    // ��������������˽�����ÿ�������������ˣ�ʱ����ms�ǣ��Ժ���������xml�ļ�����ȡ���ƣ��ڹ��캯�����ȳ�ʼ�����㣬�ٵ��ô˺���
    // ��QTimer::singleShot����ʱ������Ϣ��enemy�����ƶ������enemy��Ҫ�̳�QObject�ſ�ʹ���źźͲ�
    return true;
}

void MainWindow::doGameOver()
{
    if (!m_gameEnded)
    {
        m_gameEnded = true;
        //�˴�Ӧ�л���������������ʱ�Դ�ӡ��������paintEvent��ʵ��
    }
}

void MainWindow::updateMap()
{
//    if(!m_stopgame){//�����ͣ�Ͳ�������
        foreach (Enemy *enemy, m_enemyList)
            enemy->move();
        foreach (Tower *tower, m_towersList)
            tower->checkEnemyInRange();
        update();
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<Enemy *> MainWindow::enemyList() const//��ȡ�����
{
    return m_enemyList;
}