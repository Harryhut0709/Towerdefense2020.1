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
#include <QTimer>//ʱ��
#include <QXmlStreamReader>
#include <QtDebug>
#include <QMediaPlayer>

using namespace std;

static const int TowerCost = 300;//�趨ÿ����һ����������300���
static const int IceTowerCost = 500;//�趨ÿ����һ������������500���
static const int PoisonTowerCost = 500;//�趨ÿ����һ����������500���
static const int TowerUpgradeCost = 500;//�趨������ͨ������Ҫ500���
static const int TowerSell = 500;//����ͳһ����500���


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_waves(0)
    , m_playerHp(10)//�������������
    , m_playerGold(2000)
    , m_gameStart(true)
    , m_gameEnded(false)
    , m_gameWin(false)
{}//��ֻ��һ����,�ؿ���Ϣ�ں���Ĺ��캯�������

MainWindow1::MainWindow1(QWidget *parent)
    : MainWindow(parent)
{
    ui->setupUi(this);
    addWayPoints();
    loadTowerPositions();
    loadWave();
    loadTowerSelectButtons();//+
    loadEndButtons(); //++ //�ù���δ�ɹ�ʵ��

    m_soundcontrol = new SoundControl(this);
    m_soundcontrol->startBGM();                   //+ //���ű�������

    //һ��ѭ���ṹ��ʱ��춨��30ms��һ�Σ��ƶ����ˣ�ģ��֡��
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
}

MainWindow2::MainWindow2(QWidget *parent)
    : MainWindow(parent)//������������
{
    m_playerGold=3500;//��һ�رȽ���
    m_playerHp=10;
    ui->setupUi(this);
    addWayPoints();
    addWayPoints1();     //��ӵڶ���·��
    loadTowerPositions();
    loadWave();
    loadTowerSelectButtons();//+
    loadEndButtons(); //++

    m_soundcontrol = new SoundControl(this);
    m_soundcontrol->startBGM();                   //+ //���ű�������

    //һ��ѭ���ṹ��ʱ��춨��30ms��һ�Σ��ƶ����ˣ�ģ��֡��
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
}

void MainWindow::paintEvent(QPaintEvent *){}//��ʵ��

void MainWindow1::paintEvent(QPaintEvent *)
{
    //�Ż���Ϊ���������ӻ���
    QPixmap cachePix(":/image/setting1.jpg");
    QPainter cachePainter(&cachePix);//��ȫ�����Ƶ�cachePix��һ��ͼ��

    foreach (const TowerPosition &towerPos, m_towerPositionsList)
        towerPos.draw(& cachePainter);//�����ĳ�ʼλ��
    foreach (const Tower *tower, m_towersList)
        tower->draw(& cachePainter);//����
//    foreach (const Waypoint *waypoint, m_waypointsList)
//        waypoint->draw(& cachePainter);//��·�� //#ȡ����·�ߵĻ���
    foreach (const Enemy *enemy, m_enemyList)
        enemy->draw(& cachePainter);//������
    foreach (const Bullet *bullet, m_bulletList)
        bullet->draw(&cachePainter);//���ӵ�

    drawWave(&cachePainter);
    drawHP(&cachePainter);
    drawPlayerGold(&cachePainter);//������ʾ������Ϣ
    drawOtherTexts(&cachePainter);//+ //����������Ϣ

    QPainter painter(this);
        painter.drawPixmap(0, 0, cachePix);//�ٰ�cachePixһ���Ի�����

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
    //�Ż���Ϊ���������ӻ���
    QPixmap cachePix(":/image/setting2.jpg");
    QPainter cachePainter(&cachePix);//��ȫ�����Ƶ�cachePix��һ��ͼ��

    foreach (const TowerPosition &towerPos, m_towerPositionsList)
        towerPos.draw1(& cachePainter);//�����ĳ�ʼλ��  //�ӿӵ���״��һ��,���������µ�draw1()����
    foreach (const Tower *tower, m_towersList)
        tower->draw(& cachePainter);//����
//    foreach (const Waypoint *waypoint, m_waypointsList)
//        waypoint->draw(& cachePainter);//��·�� //#ȡ����·�ߵĻ���
//    foreach (const Waypoint *waypoint, m_waypointsList1)
//        waypoint->draw(& cachePainter);//���ڶ���·�� //#ȡ����·�ߵĻ���
    foreach (const Enemy *enemy, m_enemyList)
        enemy->draw(& cachePainter);//������
    foreach (const Bullet *bullet, m_bulletList)
        bullet->draw(&cachePainter);//���ӵ�

    drawWave(&cachePainter);
    drawHP(&cachePainter);
    drawPlayerGold(&cachePainter);//������ʾ������Ϣ
    drawOtherTexts(&cachePainter);//+ //����������Ϣ

    QPainter painter(this);
        painter.drawPixmap(0, 0, cachePix);//�ٰ�cachePixһ���Ի�����

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
        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && m_currentTowerMode==1)//+ //��ͨ��
            //��Ǯ && �������а���λ�ã����ڰ���λ���� && ����λ���ǿյ�
        {
            m_playerGold -= TowerCost;//��Ǯ
            it->setHasTower();//ռ��
            Tower *tower = new Tower (it->centerPos(),this);
            m_soundcontrol->playSound(PlantTower);//+ //����
            m_towersList.push_back(tower);
            update();
            break;
        }

        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && m_currentTowerMode==2)//+ //������
        {
            m_playerGold -= IceTowerCost;//��Ǯ
            it->setHasTower();//ռ��
            IceTower *tower = new IceTower (it->centerPos(),this);
            m_soundcontrol->playSound(PlantTower);//+ //����
            m_towersList.push_back(tower);
            update();
            break;
        }

        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && m_currentTowerMode==3)//+ //����
        {
            m_playerGold -= PoisonTowerCost;//��Ǯ
            it->setHasTower();//ռ��
            PoisonTower *tower = new PoisonTower (it->centerPos(),this);
            m_soundcontrol->playSound(PlantTower);//+ //����
            m_towersList.push_back(tower);
            update();
            break;
        }

        foreach(Tower *attacker,m_towersList)
            //�൱����˫��ѭ��,ע��towersList��towerpositionsList����������������,�����ڲ�����ʱ����Ҫ�ж�λ���Ƿ��غ�
        {
            //����
            if (attacker->containsellbutton(pressPos) && it->hasTower() && it->centerPos()==attacker->gettowerpos() )
                //�㵽��ť�� ���� ������Ϳ��Ƕ�Ӧ��
            {
                m_soundcontrol->playSound(SellTower);//����
                m_playerGold += TowerSell;//��500���
                //ȡ�������빥�����Ĺ���
                attacker->disconnectEnemyForAttack(attacker);
                m_towersList.removeOne(attacker);
                delete attacker;
                it->setHasTower(false);//����û������
                update();
                break;
            }//�������߼������� //�ᵼ������ //�ѽ��
        }

        ++it;//ע����仰��λ��
    }

    //++ ��������
//    auto upgradetect = m_towersList.begin();
//    while (upgradetect != m_towersList.end())
//    {
//        if (canUpgradeTower() && (*upgradetect)->containupgradebutton(pressPos) && !(*upgradetect)->isMaxlevel())//�㹻��� ������˰�ť�� δ����
//        {
//            m_playerGold -= TowerUpgradeCost;//��Ǯ
//            (*upgradetect)->setUpgradeTower();//����
//            (*upgradetect)->setTowerLevel();//����Ч��
//            m_soundcontrol->playSound(UpgradeTower);//����
//            update();
//            break;
//        }
//        ++upgradetect;//��һ�����ܲ���Ҫ //ȥ��������
//    }

    //++ ��������
    foreach(Tower *attacker,m_towersList)
    {
        //����
        if (m_playerGold >= TowerUpgradeCost && !attacker->isMaxlevel()
                && attacker->containupgradebutton(pressPos))//Ǯ��&&δ����&&�㵽��ť��
        {
            m_soundcontrol->playSound(UpgradeTower);//����
            m_playerGold -= TowerUpgradeCost;
            attacker->setTowerLevel(2);
            update();
            break;
        }

//        //����
//        if (attacker->containsellbutton(pressPos)&&it->hasTower())
//        {
//            m_soundcontrol->playSound(SellTower);//����
//            m_playerGold += TowerSell;//��500���
//            m_towersList.removeOne(attacker);
//            delete attacker;
//            it->setHasTower(false);//����û������
//            update();
//            break;
//        }
    }
}

void MainWindow::getHpDamage(int damage /* =1 */)
{
    m_playerHp -= damage;
    m_soundcontrol->playSound(LifeLoss);//+ //����
    if (m_playerHp<=0)
        doGameOver();
}

void MainWindow::removedEnemy(Enemy *enemy)// �з����������ߵ���ͷ����ʧ
//##���������Ҫ�޸ģ�������������Ĺ�����Χ�������һ�����㣬�򹥻����޷��жϵ����Ѿ���ʧ�����������
{
    Q_ASSERT(enemy);

    m_enemyList.removeOne(enemy);
    if (enemy->getCurrentHp()>0) m_soundcontrol->playSound(LifeLoss);//+ //û�����ߵ����أ���Ѫ
    else m_soundcontrol->playSound(EnemyKilled);                     //+ //���˱�����
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
            m_gameWin = true;
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
    m_soundcontrol->playSound(Bulletshot);//+ //�ӵ���������
    m_bulletList.push_back(bullet);
}

void MainWindow::awardGold(int gold)
{
    m_playerGold += gold;
    update();
}

bool MainWindow::canBuyTower() const //+ //�������ļ۸�һ��
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

bool MainWindow::canUpgradeTower() const //++ //�ж��Ƿ��������
{
    if (m_playerGold >= TowerUpgradeCost) return true;
    else return false;//�����ļ۸�������Ϊ��ͬ
}

void MainWindow::drawWave(QPainter *painter){}//��ʵ��

void MainWindow1::drawWave(QPainter *painter)//# //����
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

void MainWindow2::drawWave(QPainter *painter)//λ�øı���Ҫ����
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

void MainWindow::drawHP(QPainter *painter){}//��ʵ��

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

void MainWindow2::drawHP(QPainter *painter)//λ�øı���Ҫ����
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

void MainWindow::drawPlayerGold(QPainter *painter){}//��ʵ��

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

void MainWindow::drawOtherTexts(QPainter *painter){}//��ʵ��

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

void MainWindow::loadTowerPositions(){}//��ʵ��

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
    };//�������ĳ�ʼλ�ù̶����ʲ���new����
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

void MainWindow::addWayPoints(){}//��ʵ��

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

void MainWindow2::addWayPoints()//��ɣ�
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
    waypoint6->setNextWayPoint(waypoint5);//�ֲ��

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

void MainWindow2::addWayPoints1()//��ɣ�
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
    waypoint6->setNextWayPoint(waypoint5);//�ֲ��

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

void MainWindow::loadTowerSelectButtons(){}//��ʵ��

void MainWindow1::loadTowerSelectButtons()//+ //��ʼ��ѡ����ť
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
        m_currentTowerMode=1;//��ͨ��
    });

    connect(tower2,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=2;//������
    });

    connect(tower3,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=3;
    });
}

void MainWindow2::loadTowerSelectButtons()//λ�ñ仯������
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
        m_currentTowerMode=1;//��ͨ��
    });

    connect(tower2,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=2;//������
    });

    connect(tower3,&QPushButton::clicked,this,[=](){
        m_soundcontrol->playSound(SelectTower);
        m_currentTowerMode=3;
    });
}

void MainWindow::loadEndButtons(){}//������ť //��ʵ��

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

bool MainWindow::loadWave(){}//��ʵ��

bool MainWindow1::loadWave()
{
    if (m_waves>=5)//һ���岨����
    {
        return false;
    }
    Waypoint *startWaypoint = m_waypointsList.back();//֮ǰ������ʵ�֣�β�����ǿ�ʼ�ڵ�
    int enemyStartInterval[] = {1000,2500,4000,5500,7000,8500,10000,11500};
    if (m_waves==0)
    {
        for (int i=0; i<8; ++i)
        {
            Enemy *enemy = new Enemy(startWaypoint,this);
            m_enemyList.push_back(enemy);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
        }
        // ��������岨���˽�����ÿ�������������ˣ�ʱ����ms�ǣ��Ժ���������xml�ļ�����ȡ���ƣ��ڹ��캯�����ȳ�ʼ�����㣬�ٵ��ô˺���
        // ��QTimer::singleShot����ʱ������Ϣ��enemy�����ƶ������enemy��Ҫ�̳�QObject�ſ�ʹ���źźͲ�
    }//��һ������
    if (m_waves==1)
    {
        for (int i=0; i<8; ++i)
        {
            Enemy1 *enemy = new Enemy1(startWaypoint,this);
            m_enemyList.push_back(enemy);
            QTimer::singleShot(enemyStartInterval[i],enemy,SLOT(doActivate()));
        }
    }//�ڶ�������
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
    }//����������
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
    }//���Ĳ�����
    if (m_waves==4)
    {
        m_soundcontrol->playSound(FinalWave);//���һ������
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
    }//���һ��
    return true;
}

bool MainWindow2::loadWave()
{
    if (m_waves>=5)
    {
        return false;
    }
    Waypoint *startWaypoint = m_waypointsList.back();//֮ǰ������ʵ�֣�β�����ǿ�ʼ�ڵ�
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
        // ��������岨���˽�����ÿ�������������ˣ�ʱ����ms�ǣ��Ժ���������xml�ļ�����ȡ���ƣ��ڹ��캯�����ȳ�ʼ�����㣬�ٵ��ô˺���
        // ��QTimer::singleShot����ʱ������Ϣ��enemy�����ƶ������enemy��Ҫ�̳�QObject�ſ�ʹ���źźͲ�
    }//��һ������
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
    }//�ڶ�������
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
    }//����������
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
    }//���Ĳ�����
    if (m_waves==4)
    {
        m_soundcontrol->playSound(FinalWave);//���һ������
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
    }//���һ������
    return true;
}

void MainWindow::doGameOver()
{
    if (!m_gameEnded)
    {
        m_gameEnded = true;
        //�˴�Ӧ�л���������������ʱ�Դ�ӡ������paintEvent��ʵ��
    }
}

void MainWindow::updateMap()//#�޸�
{
    if(m_gameStart){//�����ͣ�Ͳ�������
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

QList<Enemy *> MainWindow::enemyList() const//��ȡ�����
{
    return m_enemyList;
}

