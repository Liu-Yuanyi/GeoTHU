#ifndef POINT_H
#define POINT_H

#include "geometricobject.h"
#include <QPointF>
#include <QColor>

class Saveloadhelper;

inline long double len(const QPointF& p){
    return sqrt(p.x()*p.x()+p.y()*p.y());
}
inline long double len(const std::pair<QPointF, QPointF> p){
    return len(p.first-p.second);
}
inline long double len2(const QPointF& p){
    return p.x()*p.x()+p.y()*p.y();
}
inline long double len2(const std::pair<QPointF, QPointF> p){
    return len2(p.first-p.second);
}

class Point : public GeometricObject {
public:

    explicit Point(const QPointF& position, bool isTemp = false);
    explicit Point(const std::vector<GeometricObject*>& parents,const int& generation);

    ObjectType getObjectType() const override { return ObjectType::Point; }
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& Pos) const override;
    QPointF position() const override;

    void setPosition(const QPointF& pos = QPointF());
    GeometricObject* flush() override;
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const override;
    ~Point();

    friend class Saveloadhelper;

private:
    QPointF PointArg;//如果是1,2,3 返回一个比例常数放在x(), 如果是4, 则为所在半径的方向向量
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
