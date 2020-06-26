//�����������Ϸ��ʼ֮ǰ�ȼ��غ÷��������ܵİڷ�λ��
#ifndef TOWERPOSITION_H
#define TOWERPOSITION_H
#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>

class TowerPosition
{
public:
    TowerPosition(QPoint pos, const QPixmap & sprite = QPixmap(":/image/towerplant.png"));

    void setHasTower(bool hasTower = true);         //���������m_hasTower
    bool hasTower() const;                          //���ش˴��Ƿ�����
    const QPoint centerPos() const;                 //�ӵ����ĵ�
    bool containPoint(const QPoint &pos) const;     //������ĵ��Ƿ��ڷ�Χ��

    void draw(QPainter *painter) const;             //���ӿ�
    void draw1(QPainter *painter) const;            //++ //�ڶ��ػ��ӿӣ���Ϊ�ӿӵ����Ӳ�һ����

private:
    QPoint        m_pos;                            //����λ�ã����Ͻ����꣩
    bool          m_hasTower;                       //��λ���Ƿ�����
    QPixmap       m_sprite;                         //�ӵ����ӣ�ͼƬ��
    QPixmap       m_sprite1;                        //�ڶ��ؿӵ�����

    static const QSize ms_fixedSize;
};

#endif // TOWERPOSITION_H
