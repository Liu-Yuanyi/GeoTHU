#ifndef POINT_H
#define POINT_H

#include "geometricobject.h"
#include <QPointF>
#include <QColor>

class Point : public GeometricObject {
public:
    explicit Point(const QPointF& position);
    QPointF position() const override { return position_; }
    void setPosition(const QPointF& pos) override;
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& Pos) const override;
    void setColor(QColor color) override;
    void setSize(double size) override;
    QColor getColor();

private:
    QPointF position_;
    double radius_;
    QColor color_;
};

#endif // POINT_H
