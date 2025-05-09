#ifndef POINT_H
#define POINT_H

#include "geometricobject.h"
#include <QPointF>
#include <QColor>

class Point : public GeometricObject {
public:

    explicit Point(const QPointF& position);

    ObjectType getObjectType() const override { return ObjectType::Point; }
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& Pos) const override;
    QPointF position() const override { return position_; }

    void setPosition(const QPointF& pos);

private:
    QPointF position_;
};

#endif // POINT_H
