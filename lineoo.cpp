#include "lineoo.h"
#include <QMessageBox>

Lineoo::Lineoo(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Line){
    parents_=parents;
    generation_=generation;
}

Qt::PenStyle Lineoo::getPenStyle()const{
    switch(shape_){
    case 0:return Qt::SolidLine;
    case 1:return Qt::DashLine;
    case 2:return Qt::DotLine;
    default:return Qt::SolidLine;
    }
}

void Lineoo::draw(QPainter* painter) const {
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
        pen.setWidth(getSize()+add+SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->drawLine(P1,P2);
    }
    pen.setColor(getColor());       // 使用基类中定义的颜色
    pen.setWidth(getSize()+add);        // 使用基类中定义的大小 (线宽)
    pen.setStyle(getPenStyle());
    painter->setPen(pen);
    painter->drawLine(P1,P2);

    return;
}

// 辅助函数：计算点 p 到直线 p1p2 的距离
double Lineoo::distanceToLineoo(const QPointF& p, const std::pair<QPointF,QPointF>& Points) const {
    double x1=Points.first.x(),x2=Points.second.x(),y1=Points.first.y(),y2=Points.first.y();
    double X=p.x(),Y=p.y();
    double A = X - x1;
    double B = Y - y1;
    double C = x2 - x1;
    double D = y2 - y1;

    // 线段长度的平方
    double dot = C * C + D * D;
    // 计算投影比例 t
    double t = (A * C + B * D) / dot;

    if (t < 0) {
        // 点在端点1的一侧
        return sqrt(A * A + B * B);
    }
    if (t > 1) {
        // 点在端点2的一侧
        return sqrt((X - x2) * (X - x2) + (Y - y2) * (Y - y2));
    }

    // 点在线段之间，计算点到直线距离
    double E = x1 + t * C;
    double F = y1 + t * D;
    return sqrt((X - E) * (X - E) + (Y - F) * (Y - F));
}

bool Lineoo::isNear(const QPointF& pos) const {
    if (isHidden()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToLineoo(pos, getTwoPoint()) < (NEAR_TOLERANCE + size_ / 2.0);
}

QPointF Lineoo::position() const {
    return getTwoPoint().first;
}

std::pair<const QPointF,const QPointF> Lineoo::getTwoPoint() const{
    switch(generation_){
    case 0:return std::make_pair(parents_[0]->position(),parents_[1]->position());
    default:
        break;
    }
    QMessageBox::warning(nullptr, "警告", "lineoo的getTwoPoint方法没有完成!");
    return std::make_pair(QPointF(),QPointF());
}

LineooCreator::LineooCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    operationName="LineooCreator";
}

std::set<GeometricObject*> LineooCreator::apply(std::vector<GeometricObject*> objs,
                                               QPointF position) const{
    Lineoo *pLineoo=new Lineoo(objs,0);
    std::set<GeometricObject*> ret{pLineoo};
    return ret;
}
