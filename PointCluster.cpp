#include "PointCluster.h"

#include <QPolygonF>
#include <algorithm>

PointCluster::PointCluster()
{

}

QVector<QPointF> PointCluster::points() const
{
    QVector<QPointF> result;
    result.resize(count());
    std::transform(begin(), end(), result.begin(), [](const Point &p) -> QPointF{
        return p.location;
    });
    return result;
}

QRectF PointCluster::boundingRect() const
{
    return QPolygonF(points()).boundingRect();
}
