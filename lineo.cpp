#include "lineo.h"
#include <QPainter>
#include <QPen>
#include <cmath>       // 用于 sqrt, fabs, pow
#include "lineoo.h"
#include "calculator.h"

void drawExtendedLineo(QPainter* painter, const QPointF& p1, const QPointF& p2) {
    // p1是射线顶点，p2是射线上的一点
    // 首先获取画布大小
    QRectF bounds = painter->viewport();
    long double width = bounds.width();
    long double height = bounds.height();

    // 计算方向向量
    long double dx = p2.x() - p1.x();
    long double dy = p2.y() - p1.y();

    // 如果两点重合，无法确定方向，直接返回
    if (is0(dx) && is0(dy)) {
        return;
    }

    // 计算射线参数方程：p = p1 + t * (dx, dy)
    // 需要找到与画布边界相交的最大t值

    long double tmax = std::numeric_limits<long double>::infinity();

    // 检查与四个边界的交点
    if (!is0(dx)) {
        // 右边界
        if (dx > 0) {
            long double t = (width - p1.x()) / dx;
            tmax = std::min(tmax, t);
        }
        // 左边界
        else if (dx < 0) {
            long double t = -p1.x() / dx;
            tmax = std::min(tmax, t);
        }
    }

    if (!is0(dy)) {
        // 下边界
        if (dy > 0) {
            long double t = (height - p1.y()) / dy;
            tmax = std::min(tmax, t);
        }
        // 上边界
        else if (dy < 0) {
            long double t = -p1.y() / dy;
            tmax = std::min(tmax, t);
        }
    }

    // 计算终点坐标
    QPointF endpoint(
        p1.x() + tmax * dx,
        p1.y() + tmax * dy
        );

    // 绘制线段
    painter->drawLine(p1, endpoint);
}

Lineo::Lineo(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Lineo){
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    GetDefaultLable[ObjectType::Lineo]=nextLineLable(GetDefaultLable[ObjectType::Lineo]);

}

Qt::PenStyle Lineo::getPenStyle()const{
    switch(shape_){
    case 0:return Qt::SolidLine;
    case 1:return Qt::DashLine;
    case 2:return Qt::DotLine;
    default:return Qt::SolidLine;
    }
}

void Lineo::draw(QPainter* painter) const {
    if (!isShown()) {
        return;
    }
    auto ppp=getTwoPoints();
    auto P1=ppp.first,P2=ppp.second;

    if (!labelhidden_ ) {
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
        drawExtendedLineo(painter,P1,P2);
    }
    pen.setColor(getColor());       // 使用基类中定义的颜色
    pen.setWidth(getSize()+add);        // 使用基类中定义的大小 (线宽)
    pen.setStyle(getPenStyle());
    painter->setPen(pen);
    drawExtendedLineo(painter,P1,P2);

    return;
}

// 辅助函数：计算点 p 到直线 p1p2 的距离
long double Lineo::distanceToLineo(const QPointF& p, const std::pair<QPointF,QPointF>& Points) const {
    long double x1=Points.first.x(),x2=Points.second.x(),y1=Points.first.y(),y2=Points.second.y();
    long double X=p.x(),Y=p.y();
    return std::abs((y2-y1)*X+(x1-x2)*Y+y1*x2-y2*x1)/std::sqrt((y2-y1)*(y2-y1)+(x1-x2)*(x1-x2));
}


bool Lineo::isNear(const QPointF& pos) const {
    if (!isShown()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToLineo(pos, getTwoPoints()) < ( 1e-2 + getSize() );
}

QPointF Lineo::position() const {
    return getTwoPoints().first;
}

inline std::pair<const QPointF,const QPointF> zhongchui(std::pair<const QPointF&,const QPointF&> ppp){
    long double x1=ppp.first.x(),x2=ppp.second.x(),y1=ppp.first.y(),y2=ppp.second.y();
    return std::make_pair(QPointF((x1+x2)/2.0,(y1+y2)/2.0),
                          QPointF((x1+x2)/2.0+y2-y1,(y1+y2)/2.0+x1-x2));
}

GeometricObject* Lineo::flush(){
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
    case 1:{
        expectParentNum(3);
        QPointF p1 = parents_[1]->position();
        QPointF a = parents_[0]->position(), b = parents_[2]->position();
        long double l1 = std::sqrt(std::pow(p1.x() - a.x(), 2) + std::pow(p1.y() - a.y(), 2));
        long double l2 = std::sqrt(std::pow(p1.x() - b.x(), 2) + std::pow(p1.y() - b.y(), 2));
        a = p1 + (a - p1) * 300 / l1;
        b = p1 + (b - p1) * 300 / l2;
        position_.push_back(p1);
        position_.push_back((a + b) / 2);
        return this;
    }
    case 2:
        // 保持原有case 2的逻辑不变（为空）
        break;
    default:
        break;
    }
    QMessageBox::warning(nullptr, "警告", "lineo的flush方法没有完成!");
    position_.push_back(QPointF());
    position_.push_back(QPointF());
    return this;
}

std::pair<const QPointF,const QPointF> Lineo::getTwoPoints() const{
    return std::make_pair(position_[0],position_[1]);
}

LineoCreator::LineoCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    operationName="LineoCreator";
}

std::set<GeometricObject*> LineoCreator::apply(std::vector<GeometricObject*> objs,
                                               QPointF position) const{
    Lineo *pLineo=new Lineo(objs,0);
    pLineo->flush();
    std::set<GeometricObject*> ret{pLineo};
    return ret;
}

std::set<GeometricObject*> LineoCreator::wait(std::vector<GeometricObject*> objs) const {
    return { new Lineo(objs, 0) };
}

bool Lineo::isTouchedByRectangle(const QPointF& start, const QPointF& end) const {
    auto p = getTwoPoints();
    long double x1 = p.first.x(), x2 = p.second.x() + 1000 * (p.second.x() - x1);
    long double y1 = p.first.y(), y2 = p.second.y() + 1000 * (p.second.y() - y1);
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
