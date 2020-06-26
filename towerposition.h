//这个类是在游戏开始之前先加载好防御塔可能的摆放位置
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

    void setHasTower(bool hasTower = true);         //建塔后更新m_hasTower
    bool hasTower() const;                          //返回此处是否有塔
    const QPoint centerPos() const;                 //坑的中心点
    bool containPoint(const QPoint &pos) const;     //所点击的点是否在范围内

    void draw(QPainter *painter) const;             //画坑坑
    void draw1(QPainter *painter) const;            //++ //第二关画坑坑（因为坑坑的样子不一样）

private:
    QPoint        m_pos;                            //塔的位置（左上角坐标）
    bool          m_hasTower;                       //该位置是否有塔
    QPixmap       m_sprite;                         //坑的样子（图片）
    QPixmap       m_sprite1;                        //第二关坑的样子

    static const QSize ms_fixedSize;
};

#endif // TOWERPOSITION_H
