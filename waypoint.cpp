#include "waypoint.h"

Waypoint::Waypoint(QPoint pos)
    :m_pos(pos)
    ,m_nextWayPoint(NULL)
{}

void Waypoint::setNextWayPoint(Waypoint *nextPoint)
{
    m_nextWayPoint = nextPoint;
}

Waypoint* Waypoint::nextWayPoint() const
{
    return m_nextWayPoint;
}

const QPoint Waypoint::pos() const
{
    return m_pos;
}

void Waypoint::draw(QPainter *painter) const
{
    painter->save();
    painter->setPen(Qt::blue);
    painter->drawEllipse(m_pos,6,6);
    painter->drawEllipse(m_pos,2,2);//��ת�۵�

    if (m_nextWayPoint)
    {
        painter->drawLine(m_pos, m_nextWayPoint->m_pos);//����������֮�����
        painter->restore();
    }
}
