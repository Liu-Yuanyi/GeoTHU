#ifndef LINE_H
#define LINE_H

#include "geometricobject.h" // 已更新以匹配提供的文件名
#include "operation.h"
#include "point.h"
#include <QPointF>
#include <QColor>
#include <QPen>   // 用于 draw 方法中的 QPen
#include <cmath>  // 用于 isNear 方法中的 std::sqrt, std::fabs

class Line : public GeometricObject {
public:
    // 构造函数
    explicit Line(const std::vector<GeometricObject*>& parents,const int& generation);

    // 重写 GeometricObject 中的纯虚函数
    ObjectType getObjectType() const override{return ObjectType::Line;}
    void draw(QPainter* painter) const override; // 绘制函数
    bool isNear(const QPointF& pos) const override; // 判断点是否在线附近
    QPointF position() const override; // 返回 startPoint_
    std::pair<const QPointF, const QPointF> getTwoPoints() const override;

protected:
    // isNear 计算的辅助函数 (点到线段的距离)
    Qt::PenStyle getPenStyle()const;
    double distanceToLine(const QPointF& p,const std::pair<QPointF,QPointF>& Points) const;
};

//line的shape:
//0实线, 1虚线, 2点线

//line的生成方式:
//0:两点连线, 1两点中垂线, 2线段中垂线, 3平行线,
//6:垂线 7圆上点切线 8圆外点切线1 9圆外点切线2
class LineCreator: public Operation{//两点连线
public:
    LineCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                               QPointF position = QPointF()) const override;
};

#endif // LINE_H
