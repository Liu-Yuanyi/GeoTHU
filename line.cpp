#include "Line.h"
#include <QPainter>
#include <QPen>
#include <cmath>       // 用于 sqrt, fabs, pow

void drawExtendedLine(QPainter* painter, const QPointF& p1, const QPointF& p2) {
    // 获取画布大小
    QRectF bounds = painter->viewport();
    qreal minX = bounds.left();
    qreal maxX = bounds.right();
    qreal minY = bounds.top();
    qreal maxY = bounds.bottom();

    // 计算直线参数 (y = kx + b)
    qreal dx = p2.x() - p1.x();
    qreal dy = p2.y() - p1.y();

    QPointF start, end;

    if (qAbs(dx) < 1e-6) {  // 垂直线
        start = QPointF(p1.x(), minY);
        end = QPointF(p1.x(), maxY);
    } else {
        qreal k = dy / dx;
        qreal b = p1.y() - k * p1.x();

        // 计算与边界的交点
        qreal x1 = minX;
        qreal y1 = k * x1 + b;
        qreal x2 = maxX;
        qreal y2 = k * x2 + b;

        // 确保交点在画布范围内
        start = QPointF(x1, qBound(minY, y1, maxY));
        end = QPointF(x2, qBound(minY, y2, maxY));
    }

    // 绘制直线
    painter->drawLine(start, end);
}

Line::Line(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Line){
    parents_=parents;
    generation_=generation;
}

Qt::PenStyle Line::getPenStyle()const{
    switch(shape_){
    case 0:return Qt::SolidLine;
    case 1:return Qt::DashLine;
    case 2:return Qt::DotLine;
    default:return Qt::SolidLine;
    }
}

void Line::draw(QPainter* painter) const {
    if (hidden_) {
        return;
    }
    if (label_ != "") {
        painter->setPen(Qt::black);
        painter->drawText(position().x() + 6, position().y() - 6, label_);
    }

    auto ppp=getTwoPoint();
    auto P1=ppp.first,P2=ppp.second;

    QPen pen; // 创建一个QPen对象用于绘制

    double add=((int)hovered_)*HOVER_ADD_WIDTH;

    if(selected_){
        pen.setColor(getColor().lighter());
        pen.setWidth(getSize()+HOVER_ADD_WIDTH+SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        drawExtendedLine(painter,P1,P2);
    }
    pen.setColor(getColor());       // 使用基类中定义的颜色
    pen.setWidth(getSize()+HOVER_ADD_WIDTH);        // 使用基类中定义的大小 (线宽)
    pen.setStyle(getPenStyle());
    painter->setPen(pen);
    drawExtendedLine(painter,P1,P2);

    return;
}

// 辅助函数：计算点 p 到直线 p1p2 的距离
double Line::distanceToLine(const QPointF& p, const std::pair<QPointF,QPointF>& Points) const {
    double x1=Points.first.x(),x2=Points.second.x(),y1=Points.first.y(),y2=Points.first.y();
    double X=p.x(),Y=p.y();
    return std::abs((y2-y1)*X+(x1-x2)*Y+y1*x2-y2*x1)/std::sqrt((y2-y2)*(y2-y1)+(x1-x2)*(x1-x2));
}


bool Line::isNear(const QPointF& pos) const {
    if (isHidden()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToLine(pos, getTwoPoint()) < (LINE_NEAR_TOLERANCE + size_ / 2.0);
}

QPointF Line::position() const {

}

std::pair<QPointF,QPointF> Line::getTwoPoint() const{

}
