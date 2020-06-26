#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
#include "soundcontrol.h"  //+
#include "button.h"        //+
#include "scene.h"
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
static const int TowerSell = 500;//拆塔统一返还500金币


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_waves(0)
    , m_playerHp(10)//调多点以免死掉
    , m_playerGold(2000)
    , m_gameStart(true)
    , m_gameEnded(false)
    , m_gameWin(false)
{}//这只是一个壳,关卡信息在后面的构造函数中填充

MainWindow1::MainWindow1(QWidget *parent)
    : MainWindow(parent)
{
    ui->setupUi(this);
    addWayPoints();
    loadTowerPositions();
    loadWave();
    loadTowerSelectButtons();//+
    loadEndButtons(); //++ //该功能未成功实现

    m_soundcontrol = new SoundControl(this);
    m_soundcontrol->startBGM();                   //+ //播放背景音乐

    //一个循环结构的时间检定，30ms做一次，移动敌人，模拟帧数
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
}

MainWindow2::MainWindow2(QWidget *parent)
    : MainWindow(parent)//这样有问题吗？
{
    m_playerGold=3500;//这一关比较难
    m_playerHp=10;
    ui->setupUi(this);
    addWayPoints();
    addWayPoints1();     //添加第二条路线
    loadTowerPositions();
    loadWave();
    loadTowerSelectButtons();//+
    loadEndButtons(); //++

    m_soundcontrol = new SoundControl(this);
    m_soundcontrol->startBGM();                   //+ //播放背景音乐

    //一个循环结构的时间检定，30ms做一次，移动敌人，模拟帧数
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
}

void MainWindow::paintEvent(QPaintEvent *){}//空实现

void MainWindow1::paintEvent(QPaintEvent *)
{
    //优化：为界面绘制添加缓存
    QPixmap cachePix(":/image/setting1.jpg");
    QPainter cachePainter(&cachePix);//先全部绘制到cachePix这一张图上

    foreach (const TowerPosition &towerPos, m_towerPositionsList)
        towerPos.draw(& cachePainter);//画塔的初始位置
    foreach (const Tower *tower, m_towersList)
        tower->draw(& cachePainter);//画塔
//    foreach (const Waypoint *waypoint, m_waypointsList)
//        waypoint->draw(& cachePainter);//画路线 //#取消对路线的绘制
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

    if (m_gameEnded || m_gameWin)
    {
        QString text = m_gameEnded? "YOU LOST!!!" : "YOU WIN!!!";
        QPainter painter(this);
        QFont font;
        font.setPointSize(20);
        font.setFamily("Segoe Script");
        font.setLetterSpacing(QFont::AbsoluteSpacing,0);
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(Qt::red);
        painter.drawText(rect(),Qt::AlignCenter,text);
        return;
    }
}

void MainWindow2::paintEvent(QPaintEvent *)
{
    //优化：为界面绘制添加缓存
    QPixmap cachePix(":/image/setting2.jpg");
    QPainter cachePainter(&cachePix);//先全部绘制到cachePix这一张图上

    foreach (const TowerPosition &towerPos, m_towerPositionsList)
        towerPos.draw1(& cachePainter);//画塔的初始位置  //坑坑的形状不一样,所以用了新的draw1()函数
    foreach (const Tower *tower, m_towersList)
        tower->draw(& cachePainter);//画塔
//    foreach (const Waypoint *waypoint, m_waypointsList)
//        waypoint->draw(& cachePainter);//画路线 //#取消对路线的绘制
//    foreach (const Waypoint *waypoint, m_waypointsList1)
//        waypoint->draw(& cachePainter);//画第二条路线 //#取消对路线的绘制
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

    if (m_gameEnded || m_gameWin)
    {
        QString text = m_gameEnded? "YOU LOST!!!" : "YOU WIN!!!";
        QPainter painter(this);
        QFont font;
        font.setPointSize(20);
        font.setFamily("Segoe Script");
        font.setLetterSpacing(QFont::AbsoluteSpacing,0);
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(Qt::red);
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

        foreach(Tower *attacker,m_towersList)
            //相当于是双重循环,注意towersList和towerpositionsList是两个独立的容器,所以在拆塔的时候需要判断位置是否重合
        {
            //拆塔
            if (attacker->containsellbutton(pressPos) && it->hasTower() && it->centerPos()==attacker->gettowerpos() )
                //点到按钮上 有塔 拆的塔和坑是对应的
            {
                m_soundcontrol->playSound(SellTower);//声音
                m_playerGold += TowerSell;//加500金币
                //取消敌人与攻击塔的关联
                attacker->disconnectEnemyForAttack(attacker);
                m_towersList.removeOne(attacker);
                delete attacker;
                it->setHasTower(false);//这里没有塔了
                update();
                break;
            }//拆塔的逻辑有问题 //会导致闪退 //已解决
        }

        ++it;//注意这句话的位置
    }

    //++ 升级功能
//    auto upgradetect = m_towersList.begin();
//    while (upgradetect != m_towersList.end())
//    {
//        if (canUpgradeTower() && (*upgradetect)->containupgradebutton(pressPos) && !(*upgradetect)->isMaxlevel())//足够金币 点击到了按钮上 未满级
//        {
//            m_playerGold -= TowerUpgradeCost;//扣钱
//            (*upgradetect)->setUpgradeTower();//升级
//            (*upgradetect)->setTowerLevel();//升级效果
//            m_soundcontrol->playSound(UpgradeTower);//声音
//            update();
//            break;
//        }
//        ++upgradetect;//这一步可能不需要 //去掉就闪退
//    }

    //++ 升级功能
    foreach(Tower *attacker,m_towersList)
    {
        //升级
        if (m_playerGold >= TowerUpgradeCost && !attacker->isMaxlevel()
                && attacker->containupgradebutton(pressPos))//钱够&&未满级&&点到按钮上
        {
            m_soundcontrol->playSound(UpgradeTower);//声音
            m_playerGold -= TowerUpgradeCost;
            attacker->setTowerLevel(2);
            update();
            break;
        }

//        //拆塔
//        if (attacker->containsellbutton(pressPos)&&it->hasTower())
//        {
//            m_soundcontrol->playSound(SellTower);//声音
//            m_playerGold += TowerSell;//加500金币
//            m_towersList.removeOne(attacker);
//            delete attacker;
//            it->setHasTower(false);//这里没有塔了
//            update();
//            break;
//        }
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
//##这个函数需要修改，否则如果有塔的攻击范围包括最后一个航点，则攻击塔无法判断敌人已经消失，会造成闪退
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
            m_gameWin = true;
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
    return false;
}

bool MainWindow::canUpgradeTower() const //++ //判断是否可以升级
{
    if (m_playerGold >= TowerUpgradeCost) return true;
    else return false;//升级的价格先设置为相同
}

void MainWindow::drawWave(QPainter *painter){}//空实现

void MainWindow1::drawWave(QPainter *painter)//# //美化
{
    painter->drawPixmap(400,5,QPixmap(":/image/waveframe.png"));
    QFont font;
    font.setPointSize(15);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(485,50, QString("%1").arg(m_waves + 1));
}

void MainWindow2::drawWave(QPainter *painter)//位置改变需要重载
{
    painter->drawPixmap(790,155,QPixmap(":/image/waveframe.png"));
    QFont font;
    font.setPointSize(15);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(870,198,QString("%1").arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter){}//空实现

void MainWindow1::drawHP(QPainter *painter)
{
    painter->drawPixmap(30,5,QPixmap(":/image/Hpframe.png"));
    QFont font;
    font.setPointSize(15);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(120,50,QString("%1").arg(m_playerHp));
}

void MainWindow2::drawHP(QPainter *painter)//位置改变需要重载
{
    painter->drawPixmap(790,5,QPixmap(":/image/Hpframe.png"));
    QFont font;
    font.setPointSize(15);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(870,48,QString("%1").arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter){}//空实现

void MainWindow1::drawPlayerGold(QPainter *painter)
{
    painter->drawPixmap(215,5,QPixmap(":/image/goldframe.png"));
    QFont font;
    font.setPointSize(15);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(275,50, QString("%1").arg(m_playerGold));
}

void MainWindow2::drawPlayerGold(QPainter *painter)
{
    painter->drawPixmap(790,80,QPixmap(":/image/goldframe.png"));
    QFont font;
    font.setPointSize(15);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(850,123,QString("%1").arg(m_playerGold));
}

void MainWindow::drawOtherTexts(QPainter *painter){}//空实现

void MainWindow1::drawOtherTexts(QPainter *painter)//+
{
    QFont font;
    font.setPointSize(10);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::red);
    painter->drawText(16,610,QString("PLEASE SELECT TOWER HERE!"));
}

void MainWindow2::drawOtherTexts(QPainter *painter)
{
    QFont font;
    font.setPointSize(10);
    font.setFamily("Segoe Script");
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::red);
    painter->drawText(600,555,QString("PLEASE SELECT TOWER HERE!"));
}

void MainWindow::loadTowerPositions(){}//空实现

void MainWindow1::loadTowerPositions()
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

void MainWindow2::loadTowerPositions()
{
    QPoint pos[] =
    {
        QPoint(150,150),
        QPoint(150,220),
        QPoint(150,290),
        QPoint(150,360),

        QPoint(50,400),

        QPoint(260,150),
        QPoint(370,150),

        QPoint(460,220),
        QPoint(460,290),
        QPoint(460,360),

        QPoint(305,290),
        QPoint(305,360),
        QPoint(305,430),

        QPoint(610,400),
        QPoint(610,330)
    };

    int len = sizeof(pos) / sizeof(pos[0]);

    for (int i=0;i<len;i++)
    {
        m_towerPositionsList.push_back(pos[i]);
    }
}

void MainWindow::addWayPoints(){}//空实现

void MainWindow1::addWayPoints()
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

void MainWindow2::addWayPoints()//完成！
{
    Waypoint *waypoint1 = new Waypoint(QPoint(780,160));
    m_waypointsList.push_back(waypoint1);

    Waypoint *waypoint2 = new Waypoint(QPoint(780,300));
    m_waypointsList.push_back(waypoint2);
    waypoint2->setNextWayPoint(waypoint1);

    Waypoint *waypoint3 = new Waypoint(QPoint(825,300));
    m_waypointsList.push_back(waypoint3);
    waypoint3->setNextWayPoint(waypoint2);

    Waypoint *waypoint4 = new Waypoint(QPoint(825,420));
    m_waypointsList.push_back(waypoint4);
    waypoint4->setNextWayPoint(waypoint3);

    Waypoint *waypoint5 = new Waypoint(QPoint(740,420));
    m_waypointsList.push_back(waypoint5);
    waypoint5->setNextWayPoint(waypoint4);

    Waypoint *waypoint6 = new Waypoint(QPoint(740,490));
    m_waypointsList.push_back(waypoint6);
    waypoint6->setNextWayPoint(waypoint5);//分叉点

    Waypoint *waypoint7 = new Waypoint(QPoint(430,490));
    m_waypointsList.push_back(waypoint7);
    waypoint7->setNextWayPoint(waypoint6);

    Waypoint *waypoint8 = new Waypoint(QPoint(430,240));
    m_waypointsList.push_back(waypoint8);
    waypoint8->setNextWayPoint(waypoint7);

    Waypoint *waypoint9 = new Waypoint(QPoint(280,240));
    m_waypointsList.push_back(waypoint9);
    waypoint9->setNextWayPoint(waypoint8);

    Waypoint *waypoint10 = new Waypoint(QPoint(280,500));
    m_waypointsList.push_back(waypoint10);
    waypoint10->setNextWayPoint(waypoint9);

    Waypoint *waypoint11 = new Waypoint(QPoint(20,500));
    m_waypointsList.push_back(waypoint11);
    waypoint11->setNextWayPoint(waypoint10);
}

void MainWindow2::addWayPoints1()//完成！
{
    Waypoint *waypoint1 = new Waypoint(QPoint(780,160));
    m_waypointsList1.push_back(waypoint1);

    Waypoint *waypoint2 = new Waypoint(QPoint(780,300));
    m_waypointsList1.push_back(waypoint2);
    waypoint2->setNextWayPoint(waypoint1);

    Waypoint *waypoint3 = new Waypoint(QPoint(825,300));
    m_waypointsList1.push_back(waypoint3);
    waypoint3->setNextWayPoint(waypoint2);

    Waypoint *waypoint4 = new Waypoint(QPoint(825,420));
    m_waypointsList1.push_back(waypoint4);
    waypoint4->setNextWayPoint(waypoint3);

    Waypoint *waypoint5 = new Waypoint(QPoint(740,420));
    m_waypointsList1.push_back(waypoint5);
    waypoint5->setNextWayPoint(waypoint4);

    Waypoint *waypoint6 = new Waypoint(QPoint(740,490));
    m_waypointsList1.push_back(waypoint6);
    waypoint6->setNextWayPoint(waypoint5);//分叉点

    Waypoint *waypoint7 = new Waypoint(QPoint(590,490));
    m_waypointsList1.push_back(waypoint7);
    waypoint7->setNextWayPoint(waypoint6);

    Waypoint *waypoint8 = new Waypoint(QPoint(590,230));
    m_waypointsList1.push_back(waypoint8);
    waypoint8->setNextWayPoint(waypoint7);

    Waypoint *waypoint9 = new Waypoint(QPoint(625,230));
    m_waypointsList1.push_back(waypoint9);
    waypoint9->setNextWayPoint(waypoint8);

    Waypoint *waypoint10 = new Waypoint(QPoint(625,110));
    m_waypointsList1.push_back(waypoint10);
    waypoint10->setNextWayPoint(waypoint9);

    Waypoint *waypoint11 = new Waypoint(QPoint(110,110));
    m_waypointsList1.push_back(waypoint11);
    waypoint11->setNextWayPoint(waypoint10);

    Waypoint *waypoint12 = new Waypoint(QPoint(110,360));
    m_waypointsList1.push_back(waypoint12);
    waypoint12->setNextWayPoint(waypoint11);

    Waypoint *waypoint13 = new Waypoint(QPoint(10,360));
    m_waypointsList1.push_back(waypoint13);
    waypoint13->setNextWayPoint(waypoint12);
}

void MainWindow::loadTowerSelectButtons(){}//空实现

void MainWindow1::loadTowerSelectButtons()//+ //初始化选塔按钮
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

void MainWindow2::loadTowerSelectButtons()//位置变化，重载
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
    tower1->move(800,475);
    tower2->move(840,470);
    tower3->move(920,490);

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

void MainWindow::loadEndButtons(){}//结束按钮 //空实现

void MainWindow1::loadEndButtons()
{
    Button * endgame = new Button(":/image/endgame.png");
    endgame->setParent(this);
    endgame->setFixedSize(68,68);
    endgame->move(870,20);

    connect(endgame,&QPushButton::clicked,this,[=](){
        Scene *menu = new Scene;
        menu->show();
        this->hide();
        delete this;
    });
}


void MainWindow2::loadEndButtons()
{
    Button * endgame = new Button(":/image/endgame.png");
    endgame->setParent(this);
    endgame->setFixedSize(68,68);
    endgame->move(20,20);

    connect(endgame,&QPushButton::clicked,this,[=](){
        Scene *menu = new Scene;
        menu->show();
        this->hide();
        delete this;
    });
}

bool MainWindow::loadWave(){}//空实现

bool MainWindow1::loadWave()
{
    if (m_waves>=5)//一共五波敌人
    {
        return false;
    }
    Waypoint *startWaypoint = m_waypointsList.back();//之前是逆序实现，尾部才是开始节点
    int enemyStartInterval[] = {1000,2500,4000,5500,7000,8500,10000,11500};
    if (m_waves==0)
    {
        for (int i=0; i<8; ++i)
        {
            Enemy *enemy = new Enemy(startWaypoint,this);
            m_enemyList.push_back(enemy);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
        }
        // 初步设计五波敌人结束，每波出现六个敌人，时间以ms记，以后这里会改用xml文件来读取控制，在构造函数中先初始化航点，再调用此函数
        // 用QTimer::singleShot来定时发送信息，enemy可以移动，因此enemy需要继承QObject才可使用信号和槽
    }//第一波敌人
    if (m_waves==1)
    {
        for (int i=0; i<8; ++i)
        {
            Enemy1 *enemy = new Enemy1(startWaypoint,this);
            m_enemyList.push_back(enemy);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
        }
    }//第二波敌人
    if (m_waves==2)
    {
        for (int i=0; i<5; ++i)
        {
            Enemy2 *enemy = new Enemy2(startWaypoint,this);
            Enemy1 *enemy1 = new Enemy1(startWaypoint,this);
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
        }
    }//第三波敌人
    if (m_waves==3)
    {
        for (int i=0; i<5; ++i)
        {
            Enemy3 *enemy = new Enemy3(startWaypoint,this);
            Enemy1 *enemy1 = new Enemy1(startWaypoint,this);
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
            if (i==2 || i==3 || i==4)
            {
                Enemy *enemy = new Enemy(startWaypoint,this);
                m_enemyList.push_back(enemy);
                QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            }
        }
    }//第四波敌人
    if (m_waves==4)
    {
        m_soundcontrol->playSound(FinalWave);//最后一波警告
        for (int i=0; i<5; ++i)
        {
            Enemy4 *enemy = new Enemy4(startWaypoint,this);
            Enemy *enemy1 = new Enemy(startWaypoint,this);
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
            if (i==2 || i==3 ||i==4)
            {
                Enemy2 *enemy = new Enemy2(startWaypoint,this);
                Enemy3 *enemy1 = new Enemy3(startWaypoint,this);
                m_enemyList.push_back(enemy);
                m_enemyList.push_back(enemy1);
                QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
                QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
            }
        }
    }//最后一波
    return true;
}

bool MainWindow2::loadWave()
{
    if (m_waves>=5)
    {
        return false;
    }
    Waypoint *startWaypoint = m_waypointsList.back();//之前是逆序实现，尾部才是开始节点
    Waypoint *startWaypoint1 = m_waypointsList1.back();
    int enemyStartInterval[] = {1000,2500,4000,5500,7000,8500,10000,11500};
    if (m_waves==0)
    {
        for (int i=0; i<4; ++i)
        {
            Enemy *enemy = new Enemy(startWaypoint,this);
            Enemy *enemy1 = new Enemy(startWaypoint1,this);
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
        }
        // 初步设计五波敌人结束，每波出现六个敌人，时间以ms记，以后这里会改用xml文件来读取控制，在构造函数中先初始化航点，再调用此函数
        // 用QTimer::singleShot来定时发送信息，enemy可以移动，因此enemy需要继承QObject才可使用信号和槽
    }//第一波敌人
    if (m_waves==1)
    {
        for (int i=0; i<8; ++i)
        {
            Enemy1 *enemy = new Enemy1(startWaypoint,this);
            Enemy1 *enemy1 = new Enemy1(startWaypoint1,this);
            Enemy *enemy2 = new Enemy(startWaypoint,this);
            Enemy *enemy3 = new Enemy(startWaypoint1,this);
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            if (3<=i && i<=6)
            {
                m_enemyList.push_back(enemy2);
                m_enemyList.push_back(enemy3);
                QTimer::singleShot(enemyStartInterval[i],enemy2,SLOT(doActivate()));
                QTimer::singleShot(enemyStartInterval[i],enemy3,SLOT(doActivate()));
            }
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
        }
    }//第二波敌人
    if (m_waves==2)
    {
        for (int i=0; i<5; ++i)
        {
            Enemy2 *enemy = new Enemy2(startWaypoint,this);
            Enemy3 *enemy1 = new Enemy3(startWaypoint1,this);
            Enemy *enemy2 = new Enemy(startWaypoint,this);
            Enemy *enemy3 = new Enemy(startWaypoint1,this);
            if (1<=i && i<=4)
            {
                m_enemyList.push_back(enemy2);
                m_enemyList.push_back(enemy3);
                QTimer::singleShot(enemyStartInterval[i],enemy2,SLOT(doActivate()));
                QTimer::singleShot(enemyStartInterval[i],enemy3,SLOT(doActivate()));
            }
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
        }
    }//第三波敌人
    if (m_waves==3)
    {
        for (int i=0; i<5; ++i)
        {
            Enemy3 *enemy = new Enemy3(startWaypoint,this);
            Enemy2 *enemy1 = new Enemy2(startWaypoint1,this);
            Enemy1 *enemy2 = new Enemy1(startWaypoint,this);
            Enemy *enemy3 = new Enemy(startWaypoint1,this);
            if (1<=i && i<=4)
            {
                m_enemyList.push_back(enemy2);
                m_enemyList.push_back(enemy3);
                QTimer::singleShot(enemyStartInterval[i],enemy2,SLOT(doActivate()));
                QTimer::singleShot(enemyStartInterval[i],enemy3,SLOT(doActivate()));
            }
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
        }
    }//第四波敌人
    if (m_waves==4)
    {
        m_soundcontrol->playSound(FinalWave);//最后一波警告
        for (int i=0; i<6; ++i)
        {
            Enemy4 *enemy = new Enemy4(startWaypoint,this);
            Enemy4 *enemy1 = new Enemy4(startWaypoint1,this);
            m_enemyList.push_back(enemy);
            m_enemyList.push_back(enemy1);
            if (i==0 || i==2 || i==4)
            {
                Enemy *enemy = new Enemy(startWaypoint,this);
                Enemy1 *enemy1 = new Enemy1(startWaypoint1,this);
                m_enemyList.push_back(enemy);
                m_enemyList.push_back(enemy1);
                QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
                QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
            }
            if (i==1 || i==3 || i==5)
            {
                Enemy2 *enemy = new Enemy2(startWaypoint,this);
                Enemy3 *enemy1 = new Enemy3(startWaypoint1,this);
                m_enemyList.push_back(enemy);
                m_enemyList.push_back(enemy1);
                QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
                QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
            }
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
            QTimer::singleShot(enemyStartInterval[i],enemy1,SLOT(doActivate()));
        }
    }//最后一波敌人
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

void MainWindow::updateMap()//#修改
{
    if(m_gameStart){//如果暂停就不更新了
        foreach (Enemy *enemy, m_enemyList)
            enemy->move();
        foreach (Tower *tower, m_towersList)
            tower->checkEnemyInRange();
        update();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow1::~MainWindow1()
{
    delete ui;
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

QList<Enemy *> MainWindow::enemyList() const//调取怪物表
{
    return m_enemyList;
}

