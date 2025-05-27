#include "Line.h"
#include <QPainter>
#include <QPen>
#include <cmath>       // 用于 sqrt, fabs, pow
#include "lineoo.h"
#include "geometricobject.h"
#include "circle.h"
#include "calculator.h"

void drawExtendedLine(QPainter* painter, const QPointF& p1, const QPointF& p2) {
    QRectF bounds = painter->viewport();
    qreal minX = bounds.left();
    qreal maxX = bounds.right();
    qreal minY = bounds.top();
    qreal maxY = bounds.bottom();

    qreal dx = p2.x() - p1.x();
    qreal dy = p2.y() - p1.y();

    // 存储所有可能的交点
    QVector<QPointF> intersections;

    // 处理垂直线
    if (qAbs(dx) < 1e-7) {
        intersections.append(QPointF(p1.x(), minY));
        intersections.append(QPointF(p1.x(), maxY));
    }
    // 处理水平线
    else if (qAbs(dy) < 1e-7) {
        intersections.append(QPointF(minX, p1.y()));
        intersections.append(QPointF(maxX, p1.y()));
    }
    // 处理斜线
    else {
        qreal k = dy / dx;
        qreal b = p1.y() - k * p1.x();

        // 与左右边界的交点
        qreal y_left = k * minX + b;
        qreal y_right = k * maxX + b;

        // 与上下边界的交点
        qreal x_top = (minY - b) / k;
        qreal x_bottom = (maxY - b) / k;

        // 检查每个交点是否在边界上
        if (y_left >= minY && y_left <= maxY)
            intersections.append(QPointF(minX, y_left));
        if (y_right >= minY && y_right <= maxY)
            intersections.append(QPointF(maxX, y_right));
        if (x_top >= minX && x_top <= maxX)
            intersections.append(QPointF(x_top, minY));
        if (x_bottom >= minX && x_bottom <= maxX)
            intersections.append(QPointF(x_bottom, maxY));
    }

    // 至少需要两个交点才能画线
    if (intersections.size() >= 2) {
        // 如果有多于两个交点，选择最远的两个
        if (intersections.size() > 2) {
            qreal maxDist = 0;
            int idx1 = 0, idx2 = 1;
            for (int i = 0; i < intersections.size(); ++i) {
                for (int j = i + 1; j < intersections.size(); ++j) {
                    QPointF diff = intersections[i] - intersections[j];
                    qreal dist = diff.x() * diff.x() + diff.y() * diff.y();
                    if (dist > maxDist) {
                        maxDist = dist;
                        idx1 = i;
                        idx2 = j;
                    }
                }
            }
            painter->drawLine(intersections[idx1], intersections[idx2]);
        } else {
            painter->drawLine(intersections[0], intersections[1]);
        }
    }
}
Line::Line(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Line){
    for(auto iter: parents){
        addParent(iter);
    }

    generation_=generation;
    GetDefaultLable[ObjectType::Line]=nextLineLable(GetDefaultLable[ObjectType::Line]);
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
    if (!isShown()) {
        return;
    }
    auto ppp=getTwoPoints();
    auto P1=ppp.first,P2=ppp.second;

    if (label_ != "") {
        painter->setPen(Qt::black);
        painter->drawText((P1.x()+P2.x())/2 + 6,
                          (P1.y()+P2.y())/2 - 6,
                          label_);
    }

    QPen pen; // 创建一个QPen对象用于绘制

    double add=((int)hovered_)*HOVER_ADD_WIDTH;

    if(selected_){
        QColor selectcolor=getColor().lighter(250);
        selectcolor.setAlpha(128);
        pen.setColor(selectcolor);
        pen.setWidth(getSize()+add+SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        drawExtendedLine(painter,P1,P2);
    }
    pen.setColor(getColor());       // 使用基类中定义的颜色
    pen.setWidth(getSize()+add);        // 使用基类中定义的大小 (线宽)
    pen.setStyle(getPenStyle());
    painter->setPen(pen);
    drawExtendedLine(painter,P1,P2);

    return;
}

// 辅助函数：计算点 p 到直线 p1p2 的距离
double Line::distanceToLine(const QPointF& p, const std::pair<QPointF,QPointF>& Points) const {
    double x1=Points.first.x(),x2=Points.second.x(),y1=Points.first.y(),y2=Points.second.y();
    double X=p.x(),Y=p.y();
    return std::abs((y2-y1)*X+(x1-x2)*Y+y1*x2-y2*x1)/std::sqrt((y2-y1)*(y2-y1)+(x1-x2)*(x1-x2));
}


bool Line::isNear(const QPointF& pos) const {
    if (!isShown()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToLine(pos, getTwoPoints()) < ( 1e-2 + size_);
}

QPointF Line::position() const {
    return getTwoPoints().first;
}

inline std::pair<const QPointF,const QPointF> zhongchui(std::pair<const QPointF&,const QPointF&> ppp){
    double x1=ppp.first.x(),x2=ppp.second.x(),y1=ppp.first.y(),y2=ppp.second.y();
    return std::make_pair(QPointF((x1+x2)/2.0,(y1+y2)/2.0),
                          QPointF((x1+x2)/2.0+y2-y1,(y1+y2)/2.0+x1-x2));
}

std::pair<const QPointF,const QPointF> Line::getTwoPoints() const{
    legal_=true;
    for(auto iter:parents_){
        if(!iter->isLegal()){
            legal_=false;
            return std::make_pair(QPointF(),QPointF(1,1));
        }
    }
    switch(generation_){
    case -3:{
        return std::make_pair(
            reflect(parents_[0]->getTwoPoints().first,parents_[1]->getTwoPoints()),
            reflect(parents_[0]->getTwoPoints().second,parents_[1]->getTwoPoints())
            );
    }
    case 0:return std::make_pair(parents_[0]->position(),parents_[1]->position());
    case 1:{
        return zhongchui(std::make_pair(parents_[0]->position(),parents_[1]->position()));
    }
    case 2:{
        return zhongchui(parents_[0]->getTwoPoints());
    }
    case 3:{//缺少三点
        QPointF P1,P2,P3;
        if(parents_[0]->getObjectType()==ObjectType::Point){
            expectParentNum(3);
            P1=parents_[0]->position();
            P2=parents_[1]->position();
            P3=parents_[2]->position();
        }
        else{
            expectParentNum(2);
            auto ppp=parents_[0]->getTwoPoints();
            P1=ppp.first,P2=ppp.second;
            P3=parents_[1]->position();
        }
        return std::make_pair(P3,QPointF(P3.x()+P1.x()-P2.x(),P3.y()+P1.y()-P2.y()));
    }
    case 6:{
        expectParentNum(2);
        QPointF P1 = parents_[0]->position();
        auto p = parents_[1]->getTwoPoints();
        QPointF P2 = p.first, P3 = p.second;
        if (P2.y() == P3.y()){
            return std::make_pair(P1, QPointF(P1.x(), P2.y() + 200));
        } else {
            return std::make_pair(P1, QPointF(P3.y() - P2.y() + P1.x(), P1.y() + P2.x() - P3.x()));
        }
    }
    case 7:{
        expectParentNum(2);
        QPointF P2 = parents_[1]->position(), P3 = parents_[0]->position();
        QPointF P1 = P3;
        if (P2.y() == P3.y()){
            return std::make_pair(P1, QPointF(P1.x(), P2.y() + 200));
        } else {
            return std::make_pair(P1, QPointF(P3.y() - P2.y() + P1.x(), P1.y() + P2.x() - P3.x()));
        }
    }
    case 8:{
        expectParentNum(2);
        Circle* circle = dynamic_cast<Circle*>(parents_[1]);
        QPointF P1 = parents_[0]->position(), P2 = circle->position();
        double radius = circle->getRadius();
        double dist1 = std::sqrt(std::pow(P1.x() - P2.x(), 2) + std::pow(P1.y() - P2.y(), 2));
        if (dist1 * dist1 - radius * radius < 0){
            legal_ = false;
            return std::make_pair(QPointF(1, 1), QPointF(2, 2));
        }
        legal_ = true;
        double dist2 = std::sqrt(dist1 * dist1 - radius * radius);
        QPointF direction1 = P2 - P1, direction2 = QPointF(-direction1.y(), direction1.x());
        QPointF P3 = P1 + direction1 * dist2 / dist1 + direction2 * radius / dist1;
        return std::make_pair(P1, P3);
    }
    case 9:{
        expectParentNum(2);
        Circle* circle = dynamic_cast<Circle*>(parents_[1]);
        QPointF P1 = parents_[0]->position(), P2 = circle->position();
        double radius = circle->getRadius();
        double dist1 = std::sqrt(std::pow(P1.x() - P2.x(), 2) + std::pow(P1.y() - P2.y(), 2));
        if (dist1 * dist1 - radius * radius < 0){
            legal_ = false;
            return std::make_pair(QPointF(1, 1), QPointF(2, 2));
        }
        legal_ = true;
        double dist2 = std::sqrt(dist1 * dist1 - radius * radius);
        QPointF direction1 = P2 - P1, direction2 = QPointF(-direction1.y(), direction1.x());
        QPointF P3 = P1 + direction1 * dist2 / dist1 - direction2 * radius / dist1;
        return std::make_pair(P1, P3);
    }
    default:
        break;
    }
    QMessageBox::warning(nullptr, "警告", "lineoo的getTwoPoint方法没有完成!");
    return std::make_pair(QPointF(),QPointF(1,1));
}

LineCreator::LineCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    operationName="LineCreator";
}

std::set<GeometricObject*> LineCreator::apply(std::vector<GeometricObject*> objs,
                                  QPointF position) const{
    Line *pLine=new Line(objs,0);
    std::set<GeometricObject*> ret{pLine};
    return ret;
}
