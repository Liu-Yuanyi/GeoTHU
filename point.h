#ifndef POINT_H
#define POINT_H

#include "geometricobject.h"
#include <QPointF>
#include <QColor>

class Point : public GeometricObject {
public:

    explicit Point(const QPointF& position);
    explicit Point(const std::vector<GeometricObject*>& parents,const int& generation);

    ObjectType getObjectType() const override { return ObjectType::Point; }
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& Pos) const override;
    QPointF position() const override{return position_;}

    void setPosition(const QPointF& pos = QPointF());

private:
    QPointF position_;
};

//point的生成方式:
/* 0自由点
1,2,3,4 line,lineo,lineoo,circle上的约束点

交点:     line    lineo   lineoo  circle
line        5       6       7       14,15
lineo       8       9       10      16,17
lineoo      11      12      13      18,19
circle                              20,21

22~29预留

中点:30
切点:31,32
圆心:33

*/

#endif // POINT_H
