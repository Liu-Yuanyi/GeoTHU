#include "lineo.h"
#include <QPainter>
#include <QPen>
#include <cmath>       // 用于 sqrt, fabs, pow
#include "lineoo.h"

void drawExtendedLineo(QPainter* painter, const QPointF& p1, const QPointF& p2) {
    // p1是射线顶点，p2是射线上的一点
    // 首先获取画布大小
    QRectF bounds = painter->viewport();
    double width = bounds.width();
    double height = bounds.height();

    // 计算方向向量
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();

    // 如果两点重合，无法确定方向，直接返回
    if (qFuzzyIsNull(dx) && qFuzzyIsNull(dy)) {
        return;
    }

    // 计算射线参数方程：p = p1 + t * (dx, dy)
    // 需要找到与画布边界相交的最大t值

    double tmax = std::numeric_limits<double>::infinity();

    // 检查与四个边界的交点
    if (!qFuzzyIsNull(dx)) {
        // 右边界
        if (dx > 0) {
            double t = (width - p1.x()) / dx;
            tmax = std::min(tmax, t);
        }
        // 左边界
        else if (dx < 0) {
            double t = -p1.x() / dx;
            tmax = std::min(tmax, t);
        }
    }

    if (!qFuzzyIsNull(dy)) {
        // 下边界
        if (dy > 0) {
            double t = (height - p1.y()) / dy;
            tmax = std::min(tmax, t);
        }
        // 上边界
        else if (dy < 0) {
            double t = -p1.y() / dy;
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
    parents_=parents;
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
    if (hidden_) {
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
double Lineo::distanceToLineo(const QPointF& p, const std::pair<QPointF,QPointF>& Points) const {
    double x1=Points.first.x(),x2=Points.second.x(),y1=Points.first.y(),y2=Points.second.y();
    double X=p.x(),Y=p.y();
    return std::abs((y2-y1)*X+(x1-x2)*Y+y1*x2-y2*x1)/std::sqrt((y2-y1)*(y2-y1)+(x1-x2)*(x1-x2));
}


bool Lineo::isNear(const QPointF& pos) const {
    if (isHidden()) return false; // 如果对象隐藏，则认为不在附近
    // 判断点到线段的距离是否小于容差值 (容差值考虑了线的厚度)
    return distanceToLineo(pos, getTwoPoints()) < ( 1e-2 + size_ / 2.0);
}

QPointF Lineo::position() const {
    return getTwoPoints().first;
}

inline std::pair<const QPointF,const QPointF> zhongchui(std::pair<const QPointF&,const QPointF&> ppp){
    double x1=ppp.first.x(),x2=ppp.second.x(),y1=ppp.first.y(),y2=ppp.second.y();
    return std::make_pair(QPointF((x1+x2)/2.0,(y1+y2)/2.0),
                          QPointF((x1+x2)/2.0+y2-y1,(y1+y2)/2.0+x1-x2));
}

std::pair<const QPointF,const QPointF> Lineo::getTwoPoints() const{
    switch(generation_){
    case 0:return std::make_pair(parents_[0]->position(),parents_[1]->position());
    case 1:{
        return zhongchui(std::make_pair(parents_[0]->position(),parents_[1]->position()));
    }
    case 2:

    default:
        break;
    }
    QMessageBox::warning(nullptr, "警告", "lineo的getTwoPoint方法没有完成!");
    return std::make_pair(QPointF(),QPointF());
}

LineoCreator::LineoCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    operationName="LineoCreator";
}

std::set<GeometricObject*> LineoCreator::apply(std::vector<GeometricObject*> objs,
                                               QPointF position) const{
    Lineo *pLineo=new Lineo(objs,0);
    std::set<GeometricObject*> ret{pLineo};
    return ret;
}
