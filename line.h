#ifndef LINE_H
#define LINE_H

#include "geometricobject.h" // 已更新以匹配提供的文件名
#include "operation.h"
#include "point.h"
#include <QPointF>
#include <QColor>
#include <QPen>   // 用于 draw 方法中的 QPen
#include <cmath>  // 用于 isNear 方法中的 std::sqrt, std::fabs

// isNear 检查用的小容差值 (例如，以像素为单位)
const double LINE_NEAR_TOLERANCE = 5.0;
const double HOVER_ADD_WIDTH =2.0;
const double SELECTED_WIDTH =2.0;

class Line : public GeometricObject {
public:
    // 构造函数
    explicit Line(const std::vector<GeometricObject*>& parents,const int& generation);

    // 重写 GeometricObject 中的纯虚函数
    ObjectType getObjectType(){return ObjectType::Line;}
    void draw(QPainter* painter) const override; // 绘制函数
    bool isNear(const QPointF& pos) const override; // 判断点是否在线附近
    QPointF position() const override; // 返回 startPoint_

private:
    // isNear 计算的辅助函数 (点到线段的距离)
    Qt::PenStyle getPenStyle()const;
    std::pair<QPointF,QPointF> getTwoPoint() const;
    double distanceToLine(const QPointF& p,const std::pair<QPointF,QPointF>& Points) const;
};

//line的shape:
//0实线, 1虚线, 2点线

//line的生成方式:
//
class LineCreator: public Operation{
public:

};

#endif // LINE_H
