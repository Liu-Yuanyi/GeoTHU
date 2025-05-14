#ifndef LINE_H
#define LINE_H

#include "geometricobject.h" // 已更新以匹配提供的文件名
#include <QPointF>
#include <QColor>
#include <QPen>   // 用于 draw 方法中的 QPen
#include <cmath>  // 用于 isNear 方法中的 std::sqrt, std::fabs

// isNear 检查用的小容差值 (例如，以像素为单位)
const double LINE_NEAR_TOLERANCE = 5.0;

class Line : public GeometricObject {
public:
    // 构造函数
    explicit Line(const QPointF& p1, const QPointF& p2);

    // 重写 GeometricObject 中的纯虚函数
    void draw(QPainter* painter) const override; // 绘制函数
    bool isNear(const QPointF& pos) const override; // 判断点是否在线附近
    void setPosition(const QPointF& pos); // 将 startPoint_ 移动到 pos，endPoint_ 相对移动
    QPointF position() const override; // 返回 startPoint_

    // Line 类特有的 getter 和 setter 方法
    QPointF getStartPoint() const { return startPoint_; } // 获取起点
    void setStartPoint(const QPointF& p); // 设置起点
    QPointF getEndPoint() const { return endPoint_; }   // 获取终点
    void setEndPoint(const QPointF& p);   // 设置终点
    void setPoints(const QPointF& p1, const QPointF& p2); // 设置起点和终点

    QColor getColor() const { return color_; }       // 获取颜色
    double getThickness() const { return thickness_; } // 获取线宽


private:
    QPointF startPoint_; // 起点
    QPointF endPoint_;   // 终点
    QColor color_;       // 颜色
    double thickness_;   // 线宽

    // isNear 计算的辅助函数 (点到线段的距离)
    double distanceToSegment(const QPointF& p, const QPointF& segP1, const QPointF& segP2) const;
};

#endif // LINE_H
