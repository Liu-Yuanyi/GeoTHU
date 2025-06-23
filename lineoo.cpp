#include "lineoo.h"
#include <QMessageBox>
#include "calculator.h"

Lineoo::Lineoo(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Lineoo){
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    GetDefaultLable[ObjectType::Lineoo]=nextLineLable(GetDefaultLable[ObjectType::Lineoo]);
}

Lineoo::Lineoo(const std::vector<QPointF>& parents) : GeometricObject(ObjectName::Lineoo){
    generation_= 100;
    position_.push_back(parents[0]);
    position_.push_back(parents[1]);
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
    if (!isShown()) {
        return;
    }
    auto ppp=getTwoPoints();
    auto P1=ppp.first,P2=ppp.second;

    if (!labelhidden_) {
        painter->setPen(Qt::black);
        painter->drawText((P1.x()+P2.x())/2 + 6,
                          (P1.y()+P2.y())/2 - 6,
                          label_);
    }



    QPen pen; // 创建一个QPen对象用于绘制

    long double add=((int)hovered_)*HOVER_ADD_WIDTH;

    if(selected_){
        QColor selectcolor=getColor().lighter(250);
        selectcolor.setAlpha(128);
        pen.setColor(selectcolor);
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
long double Lineoo::distanceToLineoo(const QPointF& p, const std::pair<QPointF,QPointF>& Points) const {
    long double x1=Points.first.x(),x2=Points.second.x(),y1=Points.first.y(),y2=Points.second.y();
    long double X=p.x(),Y=p.y();
    long double A = X - x1;
    long double B = Y - y1;
    long double C = x2 - x1;
    long double D = y2 - y1;

    // 线段长度的平方
    long double dot = C * C + D * D;
    // 计算投影比例 t
    long double t = (A * C + B * D) / dot;

    if (t < 0) {
        // 点在端点1的一侧
        return sqrt(A * A + B * B);
    }
    if (t > 1) {
        // 点在端点2的一侧
        return sqrt((X - x2) * (X - x2) + (Y - y2) * (Y - y2));
    }

    // 点在线段之间，计算点到直线距离
    long double E = x1 + t * C;
    long double F = y1 + t * D;
    return sqrt((X - E) * (X - E) + (Y - F) * (Y - F));
}

bool Lineoo::isNear(const QPointF& pos) const {
    if (!isShown()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToLineoo(pos, getTwoPoints()) < (1e-2 + getSize() );
}

QPointF Lineoo::position() const {
    return getTwoPoints().first;
}

GeometricObject* Lineoo::flush(){
    position_.clear();
    legal_=true;
    for(auto iter:parents_){
        if(!iter->isLegal()){
            legal_=false;
            position_.push_back(QPointF());
            position_.push_back(QPointF(1,1));
            return this;
        }
    }
    switch(generation_){
    case -4:{
        position_.push_back(2*parents_[1]->position()-parents_[0]->getTwoPoints().first);
        position_.push_back(2*parents_[1]->position()-parents_[0]->getTwoPoints().second);
        return this;
    }
    case -3:{
        position_.push_back(reflect(parents_[0]->getTwoPoints().first,parents_[1]->getTwoPoints()));
        position_.push_back(reflect(parents_[0]->getTwoPoints().second,parents_[1]->getTwoPoints()));
        return this;
    }
    case 0:
        position_.push_back(parents_[0]->position());
        position_.push_back(parents_[1]->position());
        return this;
    default:
        break;
    }
    QMessageBox::warning(nullptr, "警告", "lineoo的flush方法没有完成!");
    position_.push_back(QPointF());
    position_.push_back(QPointF(1,1));
    return this;
}

std::pair<const QPointF,const QPointF> Lineoo::getTwoPoints() const{
    return std::make_pair(position_[0],position_[1]);
}

LineooCreator::LineooCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    operationName="LineooCreator";
    waitImplemented = true;
}

std::set<GeometricObject*> LineooCreator::apply(std::vector<GeometricObject*> objs,
                                               QPointF position) const{
    Lineoo *pLineoo=new Lineoo(objs, 0);
    pLineoo->flush();
    std::set<GeometricObject*> ret{pLineoo};
    return ret;
}

std::set<GeometricObject*> LineooCreator::wait(std::vector<GeometricObject*> objs) const {
    return {(new Lineoo(objs, 0))};
}


bool Lineoo::isTouchedByRectangle(const QPointF& start, const QPointF& end) const {
    auto p = getTwoPoints();
    long double x1 = p.first.x(), x2 = p.second.x();
    long double y1 = p.first.y(), y2 = p.second.y();
    long double a = y1 - y2, b = x2 - x1, c = y2*x1 - y1*x2;
    std::vector<QPointF> v = {start, end, QPointF(start.x(), end.y()), QPointF(end.x(), start.y())};
    std::set<int> sgn;
    for (auto point : v){
        if (a * point.x() + b * point.y() + c > 0){
            sgn.insert(1);
        } else if (a * point.x() + b * point.y() + c == 0){
            sgn.insert(0);
        } else {
            sgn.insert(-1);
        }
    }
    if (sgn.size() < 2){
        return false;
    }
    long double largerX = std::max(start.x(), end.x()), smallerX = std::min(start.x(), end.x());
    long double largerY = std::max(start.y(), end.y()), smallerY = std::min(start.y(), end.y());
    if ((x1 > largerX and x2 > largerX) or (x1 < smallerX and x2 < smallerX) or
        (y1 > largerY and y2 > largerY) or (y1 < smallerY and y2 < smallerY)){
        return false;
    }
    return true;
}
