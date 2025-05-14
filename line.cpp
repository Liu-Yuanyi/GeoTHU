#include "Line.h"
#include <QPainter>
#include <QPen>
#include <cmath>       // 用于 sqrt, fabs, pow
#include <algorithm>   // 用于 std::min, std::max

Line::Line(const QPointF& p1, const QPointF& p2)
    : GeometricObject(ObjectName::Line), startPoint_(p1), endPoint_(p2) {}

void Line::draw(QPainter* painter) const {
    if (isHidden()) { // 使用基类的 getter 方法
        return;
    }

    painter->save(); // 保存 QPainter 的当前状态

    QPen pen(color_, thickness_); // 使用 Line 类的 color_ 和 thickness_ 成员
    pen.setCapStyle(Qt::RoundCap); // 使线端看起来更圆润一些

    if (isHovered()) { // 使用基类的 getter 方法
        QPen hoverPen(color_, thickness_ + 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // 悬停时线条更粗
        if (isSelected()) { // 使用基类的 getter 方法
            hoverPen.setColor(Qt::darkRed); // 如果同时被选中，则使用不同的颜色
        } else {
            hoverPen.setColor(Qt::red); // 悬停颜色
        }
        painter->setPen(hoverPen);
    } else if (isSelected()) { // 使用基类的 getter 方法
        QPen selectedPen(Qt::darkRed, thickness_ + 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin); // 选中时使用虚线
        painter->setPen(selectedPen);
    } else {
        painter->setPen(pen); // 默认画笔
    }

    painter->drawLine(startPoint_, endPoint_);

    // 可选：绘制标签 (如果标签不为空)
    if (!getLabel().isEmpty()) { // 使用基类的 getter 方法
        painter->setPen(Qt::black); // 重置画笔颜色用于文本，或使用特定的文本颜色
        // 将标签放置在线段中点或起点附近
        QPointF labelPos = (startPoint_ + endPoint_) / 2.0 + QPointF(5, -5); // 标签位置偏移量
        painter->drawText(labelPos, getLabel());
    }

    painter->restore(); // 恢复 QPainter 的状态
}

// 辅助函数：计算点 p 到线段 p1p2 的距离
double Line::distanceToSegment(const QPointF& p, const QPointF& segP1, const QPointF& segP2) const {
    // 计算线段长度的平方
    double l2 = (segP2.x() - segP1.x()) * (segP2.x() - segP1.x()) + (segP2.y() - segP1.y()) * (segP2.y() - segP1.y());
    if (l2 == 0.0) return std::sqrt((p.x() - segP1.x()) * (p.x() - segP1.x()) + (p.y() - segP1.y()) * (p.y() - segP1.y())); // segP1 和 segP2 是同一点

    // 考虑线段所在直线为 P = segP1 + t * (segP2 - segP1)
    // t = [(p - segP1) . (segP2 - segP1)] / |segP2 - segP1|^2  (点积运算)
    double t = ((p.x() - segP1.x()) * (segP2.x() - segP1.x()) + (p.y() - segP1.y()) * (segP2.y() - segP1.y())) / l2;
    t = std::max(0.0, std::min(1.0, t)); // 将 t 限制在 [0,1] 区间内，以确保投影点在线段上

    QPointF projection = segP1 + t * (segP2 - segP1); // 计算点 p 在线段上的投影点
    // 返回点 p 到投影点的距离
    return std::sqrt(std::pow(p.x() - projection.x(), 2) + std::pow(p.y() - projection.y(), 2));
}


bool Line::isNear(const QPointF& pos) const {
    if (isHidden()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToSegment(pos, startPoint_, endPoint_) < (LINE_NEAR_TOLERANCE + thickness_ / 2.0);
}

void Line::setPosition(const QPointF& pos) {
    QPointF offset = pos - startPoint_; // 计算偏移量
    startPoint_ = pos;                  // 将起点移动到新位置
    endPoint_ += offset;                // 终点相对于起点进行等量移动
}

QPointF Line::position() const {
    return startPoint_; // 将线对象的“位置”定义为其起点
}

void Line::setStartPoint(const QPointF& p) {
    startPoint_ = p;
}

void Line::setEndPoint(const QPointF& p) {
    endPoint_ = p;
}

void Line::setPoints(const QPointF& p1, const QPointF& p2) {
    startPoint_ = p1;
    endPoint_ = p2;
}
