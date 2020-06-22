#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
#include "soundcontrol.h"  //+
#include "button.h"        //+
//#include "plistreader.h"
#include <QPainter>
#include <iostream>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>//时间
#include <QXmlStreamReader>
#include <QtDebug>
#include <QMediaPlayer>

using namespace std;

static const int TowerCost = 300;//设定每安置一个炮塔花费300金币
static const int IceTowerCost = 500;//设定每安置一个冰冻塔花费500金币
static const int PoisonTowerCost = 500;//设定每安置一个毒塔花费500金币
static const int TowerUpgradeCost = 500;//设定升级普通炮塔需要500金币


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_waves(0)
    , m_playerHp(5)//调多点以免死掉
    , m_playerGold(20000)//先开挂
    , m_gameEnded(false)
    , m_gameWin_first(false)
{
    ui->setupUi(this);
    addWayPoints();
    loadTowerPositions();
    loadWave();
    loadTowerSelectButtons();//+

    m_soundcontrol = new SoundControl(this);
    m_soundcontrol->startBGM();                   //+ //播放背景音乐

    //一个循环结构的时间检定，30ms做一次，移动敌人，模拟帧数
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //优化：为界面绘制添加缓存
    QPixmap cachePix(":/image/setting1.jpg");
    QPainter cachePainter(&cachePix);//先全部绘制到cachePix这一张图上

    foreach (const TowerPosition &towerPos, m_towerPositionsList)
        towerPos.draw(& cachePainter);//画塔的初始位置
    foreach (const Tower *tower, m_towersList)
        tower->draw(& cachePainter);//画塔
    foreach (const Waypoint *waypoint, m_waypointsList)
        waypoint->draw(& cachePainter);//画路线
    foreach (const Enemy *enemy, m_enemyList)
        enemy->draw(& cachePainter);//画敌人
    foreach (const Bullet *bullet, m_bulletList)
        bullet->draw(&cachePainter);//画子弹

    drawWave(&cachePainter);
    drawHP(&cachePainter);
    drawPlayerGold(&cachePainter);//顶端显示基本信息
    drawOtherTexts(&cachePainter);//+ //其他文字信息

    QPainter painter(this);
        painter.drawPixmap(0, 0, cachePix);//再把cachePix一次性画出来

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
        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && m_currentTowerMode==1)//+ //普通塔
            //有钱 && 遍历所有安放位置，点在安放位置内 && 安放位置是空的
        {
            m_playerGold -= TowerCost;//扣钱
            it->setHasTower();//占坑
            Tower *tower = new Tower (it->centerPos(),this);
            m_soundcontrol->playSound(PlantTower);//+ //声音
            m_towersList.push_back(tower);
            update();
            break;
        }

        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && m_currentTowerMode==2)//+ //冰冻塔
        {
            m_playerGold -= IceTowerCost;//扣钱
            it->setHasTower();//占坑
            IceTower *tower = new IceTower (it->centerPos(),this);
            m_soundcontrol->playSound(PlantTower);//+ //声音
            m_towersList.push_back(tower);
            update();
            break;
        }

        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && m_currentTowerMode==3)//+ //毒塔
        {
            m_playerGold -= PoisonTowerCost;//扣钱
            it->setHasTower();//占坑
            PoisonTower *tower = new PoisonTower (it->centerPos(),this);
            m_soundcontrol->playSound(PlantTower);//+ //声音
            m_towersList.push_back(tower);
            update();
            break;
        }
        ++it;
    }

    //++ 升级功能
    auto upgradetect = m_towersList.begin();
    while (upgradetect != m_towersList.end())
    {
        if (canUpgradeTower() && (*upgradetect)->containupgradebutton(pressPos) && !(*upgradetect)->isMaxlevel())//足够金币 点击到了按钮上 未满级
        {
            m_playerGold -= TowerUpgradeCost;//扣钱
            (*upgradetect)->setUpgradeTower();//升级
            (*upgradetect)->setTowerLevel();//升级效果
            m_soundcontrol->playSound(PlantTower);//声音，先暂时用建塔的声音
            update();
            break;
        }
        ++upgradetect;//这一步可能不需要 //去掉就闪退
    }

}

void MainWindow::getHpDamage(int damage /* =1 */)
{
    m_playerHp -= damage;
    m_soundcontrol->playSound(LifeLoss);//+ //声音
    if (m_playerHp<=0)
        doGameOver();
}

void MainWindow::removedEnemy(Enemy *enemy)// 敌方被攻击或走到尽头，消失
{
    Q_ASSERT(enemy);

    m_enemyList.removeOne(enemy);
    if (enemy->getCurrentHp()>0) m_soundcontrol->playSound(LifeLoss);//+ //没打死走到基地，扣血
    else m_soundcontrol->playSound(EnemyKilled);                     //+ //敌人被击毙
    delete enemy;

    // 没有怪物就加一波
    if (m_enemyList.empty())
    {
        ++m_waves;// 当前波数+1
        // 继续读取下一波
        if (!loadWave())
        {
            // 当没有下一波时，这里表示游戏胜利
            // 设置游戏胜利标志为true
            m_gameWin_first = true;
            // 游戏胜利转到游戏胜利场景（这里只实现了第一关）
            // 没加胜利效果
        }
    }
}

void MainWindow::removedBullet(Bullet *bullet)//子弹打中敌人，消失
{
    Q_ASSERT(bullet);

    m_bulletList.removeOne(bullet);
    delete bullet;
}

void MainWindow::addBullet(Bullet *bullet)
{
    Q_ASSERT(bullet);
    m_soundcontrol->playSound(Bulletshot);//+ //子弹发射声音
    m_bulletList.push_back(bullet);
}

void MainWindow::awardGold(int gold)
{
    m_playerGold += gold;
    update();
}

bool MainWindow::canBuyTower() const //+ //三种塔的价格不一样
{
    if (m_currentTowerMode==1)
    {
        if (m_playerGold >= TowerCost) return true;
        else return false;
    }
    else if (m_currentTowerMode==2)
    {
        if (m_playerGold >= IceTowerCost) return true;
        else return false;
    }
    else if (m_currentTowerMode==3)
    {
        if (m_playerGold >= PoisonTowerCost) return true;
        else return false;
    }
}

bool MainWindow::canUpgradeTower() const //++ //判断是否可以升级
{
    if (m_playerGold >= TowerUpgradeCost) return true;
    else return false;//升级的价格先设置为相同
}

void MainWindow::drawWave(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(400,5,100,25), QString("WAVE : %1").arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(30,5,100,25), QString("HP : %1").arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(200,5,200,25), QString("GOLD : %1").arg(m_playerGold));
}

void MainWindow::drawOtherTexts(QPainter *painter)//+
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(60,500,300,25), QString("PLEASE SELECT TOWER HERE!"));
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
    };//由于塔的初始位置固定，故不用new方法
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

void MainWindow::loadTowerSelectButtons()//+ //初始化选塔按钮
{
    Button *tower1 = new Button(":/image/tower1.png");
    Button *tower2 = new Button(":/image/tower2.png");
    Button *tower3 = new Button(":/image/tower3.png");
    tower1->setParent(this);
    tower2->setParent(this);
    tower3->setParent(this);
    tower1->setFixedSize(32,64);
    tower2->setFixedSize(72,72);
    tower3->setFixedSize(32,32);
    tower1->move(80,530);
    tower2->move(120,525);
    tower3->move(200,545);

    connect(tower1,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=1;//普通塔
    });

    connect(tower2,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=2;//冰冻塔
    });

    connect(tower3,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=3;
    });
}

bool MainWindow::loadWave()
{
    if (m_waves>=6)
    {
        return false;
    }
    Waypoint *startWaypoint = m_waypointsList.back();//之前是逆序实现，尾部才是开始节点
    int enemyStartInterval[] = {1000,2500,4000,5500,7000,8500,10000,11500};
    for (int i=0; i<8; ++i)
    {
        Enemy *enemy = new Enemy(startWaypoint,this);
        m_enemyList.push_back(enemy);
        QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
    }
    // 初步设计六波敌人结束，每波出现八个敌人，时间以ms记，以后这里会改用xml文件来读取控制，在构造函数中先初始化航点，再调用此函数
    // 用QTimer::singleShot来定时发送信息，enemy可以移动，因此enemy需要继承QObject才可使用信号和槽
    return true;
}

void MainWindow::doGameOver()
{
    if (!m_gameEnded)
    {
        m_gameEnded = true;
        //此处应切换到结束场景，暂时以打印处理，在paintEvent中实现
    }
}

void MainWindow::updateMap()
{
//    if(!m_gameStop){//如果暂停就不更新了
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

QList<Enemy *> MainWindow::enemyList() const//调取怪物表
{
    return m_enemyList;
}
