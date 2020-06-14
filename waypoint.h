//������ǶԵ�ͼ�ϵĹյ���к�������
#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <QMainWindow>
#include <QObject>
#include <QPoint>
#include <QPainter>
#include <QVector2D>

class Waypoint
{
public:
    Waypoint(QPoint pos);

    void setNextWayPoint(Waypoint *nextPoint);
    Waypoint* nextWayPoint() const;
    const QPoint pos() const;

    void draw(QPainter *painter) const;

private:
    const QPoint		m_pos;
    Waypoint *			m_nextWayPoint;
};

#endif // WAYPOINT_H
