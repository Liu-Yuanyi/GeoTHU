#ifndef POINT_H
#define POINT_H

#include "geometricobject.h"
#include <QPointF>
#include <QColor>

class Saveloadhelper;


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

交点:     line    lineo   lineoo  circle    Arc
line        5       6       7       14,15   34,35
lineo       8       9       10      16,17   36,37
lineoo      11      12      13      18,19   38,39
circle                              20,21   40,41
Arc                                         42,43

23~27预留

28 圆弧上的约束点
29 圆弧的另一个端点

中点:30
切点:31,32
圆心:33(未实现)



*/

#endif // POINT_H
