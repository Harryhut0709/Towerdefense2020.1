//Ŀǰֻ��һ����ͨ�ӵ�normalbullet
//֮��Ԥ�Ƽ�������������ӵ��������ӵ�icebullet�����٣����ж��ӵ�poisonbullet�������˺���

#ifndef BULLET_H
#define BULLET_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>

class QPainter;
class Enemy;
class MainWindow;

class Bullet :public QObject
{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)//QT����Ч��
    //ע��ΪQT���ԣ����ӵ����ɺ����move()������ʹ���ӵ������Զ�����Ч��

public:
    Bullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/normalbullet.png"));

    void draw(QPainter *painter) const;       //���ӵ�
    void move();                              //ʵ���ӵ�����Ч��������qt�Դ���animation����
    void setCurrentPos(QPoint pos);           //�����ӵ�ʵʱλ��

    QPoint currentPos() const;                //��ȡ�ӵ�ʵʱλ��

public slots:
    virtual void hitTarget();                 //�ۺ��������е���ʱ����������ӣ�ʹ�õ����ܵ��˺�

protected:
    const QPoint	m_startPos;               //��������λ�ã��ӵ���ʼ�㣩
    const QPoint	m_targetPos;              //����λ�ã��ӵ�Ŀ�ĵأ�
    const QPixmap	m_sprite;                 //�ӵ���ʽ
    QPoint			m_currentPos;             //��ǰλ�ã����ϸ��£�
    Enemy *			m_target;                 //Ҫ�����ĵ���
    MainWindow *	m_game;                   //����MainWindowʵ��paintevent
    int				m_damage;                 //�ɹ����������Ծ���

    static const QSize ms_fixedSize;
};

//+
class IceBullet:public Bullet{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
    IceBullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/icebullet.png"));
    ~IceBullet();

public slots:
    void hitTarget();                 //�ۺ��������е���ʱ����������ӣ�ʹ�õ����ܵ��˺�
    //ͬʱ�ı䱻���б����е�enemy��m_isSlowed
};

class PoisonBullet:public Bullet{
    Q_OBJECT//����Ӧ��QT�۵��඼��Ҫ����
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
    PoisonBullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/poisonbullet.png"));
    ~PoisonBullet();

public slots:
    void hitTarget();                 //�ۺ��������е���ʱ����������ӣ�ʹ�õ����ܵ��˺�
    //ͬʱ�ı䱻���б����е�enemy��m_isPoisoned
};

#endif // BULLET_H
