#ifndef POINTCLUSTER_H
#define POINTCLUSTER_H

#include <QVector>
#include <QPointF>
#include <QRectF>

typedef struct {
    QPointF location;
    long level;
    unsigned int index;
} Point;

class PointCluster: public QVector<Point>
{
public:
    PointCluster();

    QRectF boundingRect() const;
    QVector<QPointF> points() const;
};

#endif // POINTCLUSTER_H
