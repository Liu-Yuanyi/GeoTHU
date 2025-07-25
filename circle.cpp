#include "circle.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <cmath> // For std::pow, std::sqrt
#include <QMessageBox>
#include "point.h"
#include "lineoo.h"
#include "calculator.h"
// 初始化全局默认值 (如果需要，这些通常在主程序或特定初始化函数中完成，
// 但这里作为示例，假设你需要在objecttype.cpp或类似地方添加Circle的默认值)
// extern std::map<ObjectType, QString> GetDefaultLable;
// extern std::map<ObjectType, QColor> GetDefaultColor;
// extern std::map<ObjectType, double> GetDefaultSize;
// extern std::map<ObjectType, int> GetDefaultShape;
// ... 你可能需要在某处初始化这些map中关于Circle的条目 ...

Qt::PenStyle Circle::getPenStyle() const {
    // 将整数值转换为 Qt::PenStyle
    switch (shape_) {
    case 0: // Solid
        return Qt::SolidLine;
    case 1: // Dash
        return Qt::DashLine;
    case 2: // Dot
        return Qt::DotLine;
    default:
        return Qt::SolidLine;
    }
}
Qt::PenStyle Arc::getPenStyle() const {
    // 将整数值转换为 Qt::PenStyle
    switch (shape_) {
    case 0: // Solid
        return Qt::SolidLine;
    case 1: // Dash
        return Qt::DashLine;
    case 2: // Dot
        return Qt::DotLine;
    default:
        return Qt::SolidLine;
    }
}

Circle::Circle(const std::vector<GeometricObject*>& parents, const int& generation, bool isTemp, bool aux)
    : GeometricObject(ObjectName::Circle, aux){
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    if (!isTemp and !aux) {
        GetDefaultLable[ObjectType::Circle]=nextLineLable(GetDefaultLable[ObjectType::Circle]);
    }
}

Arc::Arc(const std::vector<GeometricObject*>& parents, const int& generation, bool isTemp, bool aux)
    : GeometricObject(ObjectName::Arc, aux){
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    if (!isTemp and !aux) {
        GetDefaultLable[ObjectType::Arc]=nextLineLable(GetDefaultLable[ObjectType::Arc]);
    }
}

void Circle::draw(QPainter* painter) const {
    if (!isShown()) return;

    // 获取圆心和半径
    auto points = getTwoPoints();
    QPointF center = points.first;
    long double radius = QLineF(points.first, points.second).length();

    QPen pen;
    long double add = ((int)hovered_) * HOVER_ADD_WIDTH;

    // 如果被选中，先绘制一个较宽的选中效果
    if (selected_) {
        QColor selectcolor = getColor().lighter(250);
        selectcolor.setAlpha(128);
        pen.setColor(selectcolor);
        pen.setWidthF(getSize() + add + SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        painter->drawEllipse(center, radius, radius);

    }

    // 设置正常绘制的画笔
    pen.setColor(getColor());
    pen.setWidthF(getSize() + add); // 添加 hovered 状态的额外宽度
    pen.setStyle(getPenStyle());    // 假设您有 getPenStyle() 方法
    painter->setPen(pen);

    // 根据圆的类型绘制不同形状

        painter->drawEllipse(center, radius, radius);
        // 添加标签绘制（如果有）
        if (!labelhidden_) {
            painter->setPen(Qt::black);
            painter->drawText(center.x() + radius + 6,
                              center.y() - 6,
                              label_);
        }

}
void Arc::draw(QPainter* painter) const {

    if (!isShown()) return;

    qDebug()<<"theta(p0-p1)="<<(double)Theta(getTwoPoints().first-getTwoPoints().second);

    // 获取圆心和半径
    auto points = getTwoPoints();
    QPointF center = points.first;
    long double radius = QLineF(points.first, points.second).length();
    std::pair<long double,long double> Angles = getAngles();
    qDebug()<<"A= "<<parents_[0]->position()<<" B= "<<parents_[1]->position();
    qDebug()<<"Angles= [ "<<(double)Angles.first<<","<<(double)Angles.second<<" ]";

    int startAngleQt = (Angles.first) * 180 / PI * 16;
    int spanAngleQt = (Angles.second - Angles.first) * 180 / PI * 16;
    if(spanAngleQt<0){ spanAngleQt += 360*16; }
    if(spanAngleQt>=360*16){ spanAngleQt -= 360*16; }
    QRectF rect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
    QPen pen;
    long double add = ((int)hovered_) * HOVER_ADD_WIDTH;

    // 如果被选中，先绘制一个较宽的选中效果
    if (selected_) {
        QColor selectcolor = getColor().lighter(250);
        selectcolor.setAlpha(128);
        pen.setColor(selectcolor);
        pen.setWidthF(getSize() + add + SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        painter->drawArc(rect, startAngleQt, spanAngleQt);
    }

    // 设置正常绘制的画笔
    pen.setColor(getColor());
    pen.setWidthF(getSize() + add); // 添加 hovered 状态的额外宽度
    pen.setStyle(getPenStyle());    // 假设您有 getPenStyle() 方法
    painter->setPen(pen);

    // 根据圆的类型绘制不同形状

    painter->drawArc(rect, startAngleQt, spanAngleQt);
    // 添加标签绘制（如果有）
    if (!labelhidden_) {
        painter->setPen(Qt::black);
        painter->drawText(center.x() + radius*cos(startAngleQt+16*10) + 6,
                          center.y() + radius*sin(startAngleQt+16*10)- 6,
                          label_);
    }

}


bool Circle::isNear(const QPointF& pos) const {
    if (!isShown()) return false;
    return (abs(len(pos-position())-len(getTwoPoints()))<getSize()+1e-2);
}

bool Arc::isNear(const QPointF& pos) const {
    if (!isShown()) return false;

    auto [s,t] = getAngles();
    long double theta=Theta(pos-position());

    return (abs(len(pos-position())-len(getTwoPoints()))<getSize()+1e-2) &&
           thetainst(theta,s,t);
}

GeometricObject* Circle::flush(){
    position_.clear();
    legal_ = true;
    for (auto iter : parents_) {
        if (!iter->isLegal()) {
            legal_ = false;
            position_.push_back(QPointF());position_.push_back(QPointF(1, 1));
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
    case 0:{
        position_.push_back(parents_[0]->position()),position_.push_back(parents_[1]->position());
        return this;
    }
    case 1:{
        if(parents_.size()==2){
            position_.push_back(parents_[0]->position());
            position_.push_back(parents_[0]->position()+QPointF(dynamic_cast<Lineoo*>(parents_[1])->length(),0));
        }
        else{
            position_.push_back(parents_[0]->position());
            position_.push_back(parents_[0]->position()+QPointF(len(parents_[1]->position()-parents_[2]->position()),0));
        }
        return this;
    }
    case 2:{
        position_.push_back(calculateCircleCenter(parents_[0]->position(),parents_[1]->position(),parents_[2]->position())),position_.push_back(parents_[0]->position());
        return this;
    }
    default:{
        QMessageBox::warning(nullptr,"警告","Cirle的flush方法未实现!");
        position_.push_back(QPointF());position_.push_back(QPointF(1, 1));
        return this;
    }
    }
}
GeometricObject* Arc::flush(){
    position_.clear();
    legal_ = true;
    for (auto iter : parents_) {
        if (!iter->isLegal()) {
            legal_ = false;
            position_.push_back(QPointF());position_.push_back(QPointF(1, 1));
            return this;
        }
    }
    /*
    QPointF p= QPointF(1.0,0.01);
    qDebug()<<(double)Theta(p);qDebug()<<(double)(Theta(p)/PI)<<" pi";*/

    switch (generation_) {
    case -4:{
        position_.push_back(2*parents_[1]->position()-parents_[0]->getTwoPoints().first);
        position_.push_back(2*parents_[1]->position()-parents_[0]->getTwoPoints().second);
        Angles_.second=normalizeAngle(PI+dynamic_cast<Arc*>(parents_[0])->getAngles().second);
        Angles_.first=normalizeAngle(PI+dynamic_cast<Arc*>(parents_[0])->getAngles().first);
        return this;
    }
    case -3:{
        position_.push_back(reflect(parents_[0]->getTwoPoints().first,parents_[1]->getTwoPoints()));
        position_.push_back(reflect(parents_[0]->getTwoPoints().second,parents_[1]->getTwoPoints()));
        long double reflectAngle = Theta(parents_[1]->getTwoPoints());
        Angles_.second=normalizeAngle(normalizeAngle(2*reflectAngle-dynamic_cast<Arc*>(parents_[0])->getAngles().first));
        Angles_.first=normalizeAngle(normalizeAngle(2*reflectAngle-dynamic_cast<Arc*>(parents_[0])->getAngles().second));
        return this;
    }
    case 0:{
        position_.push_back((parents_[0]->position()+parents_[1]->position())/2);
        position_.push_back(parents_[0]->position());
        Angles_.first= Theta(parents_[0]->position()-parents_[1]->position());
        Angles_.second=(Angles_.first>= PI? Angles_.first- PI: Angles_.first+ PI);
        return this;
    }
    case 1:{
        position_.push_back(parents_[0]->position());
        position_.push_back(parents_[1]->position());
        Angles_.first = Theta(parents_[1]->position()-parents_[0]->position());
        Angles_.second = Theta(parents_[2]->position()-parents_[0]->position());
        return this;
    }
    default:{
        QMessageBox::warning(nullptr,"警告","Cirle的flush方法未实现!");
        position_.push_back(QPointF());position_.push_back(QPointF(1, 1));
        return this;
    }
    }
}
std::pair<long double, long double> Arc::getAngles() const{
    return Angles_;
}
QPointF Circle::position() const {
    return getTwoPoints().first;
}
QPointF Arc::position() const {
    return getTwoPoints().first;
}

long double Circle::getRadius() const {
    const auto& points = getTwoPoints();
    return len(points.first - points.second);
}
long double Arc::getRadius() const {
    const auto& points = getTwoPoints();
    return len(points.first - points.second);
}

std::pair<const QPointF, const QPointF> Circle::getTwoPoints() const {
    return std::make_pair(position_[0],position_[1]);
}
std::pair<const QPointF, const QPointF> Arc::getTwoPoints() const {
    return std::make_pair(position_[0],position_[1]);
}

bool Circle::isTouchedByRectangle(const QPointF& start, const QPointF& end) const {
    auto p = getTwoPoints();
    long double dist = QLineF(p.first, p.second).length();
    long double x = p.first.x(), y = p.first.y();
    std::vector<long double> v = {start.x(), end.y(), end.x(), start.y()};
        std::vector<long double> w = {x, y};
        long double minDist = 1e9L, maxDist = 0L;
        for (int i = 0; i < 4; ++i){
                maxDist = std::max(len(QPointF(v[2 * (i/2)], v[2 * (i%2) + 1])-p.first), maxDist);
            }
        long double minX = std::min(std::abs(start.x() - x), std::abs(end.x() - x));
        long double minY = std::min(std::abs(start.y() - y), std::abs(end.y() - y));
        if ((start.y() - y) * (end.y() - y) <= 0){
                minDist = std::min(minDist, minX);
            } else{
                minDist = std::min(minDist, std::sqrt(minX * minX + minY * minY));
            }
        if ((start.x() - x) * (end.x() - x) <= 0){
                minDist = std::min(minDist, minY);
            } else{
                minDist = std::min(minDist, std::sqrt(minX * minX + minY * minY));
            }
        if ((start.y() - y) * (end.y() - y) <= 0 and (start.x() - x) * (end.x() - x) <= 0){
                minDist = 0;
    }
    return maxDist >= dist and minDist <= dist;
}
bool Arc::isTouchedByRectangle(const QPointF& start, const QPointF& end) const {
    long double left = std::min(start.x(), end.x());
    long double right = std::max(start.x(), end.x());
    long double top = std::min(start.y(), end.y());
    long double bottom = std::max(start.y(), end.y());

    // 3. 检查圆弧端点是否在矩形内
    long double radius=len(getTwoPoints());
    long double x = position().x() + radius * cos(Angles_.first);
    long double y = position().y() - radius * sin(Angles_.first); // 注意y向下为正
    if (x >= left && x <= right && y >= top && y <= bottom)
        return true;
    x = position().x() + radius * cos(Angles_.second);
    y = position().y() - radius * sin(Angles_.second); // 注意y向下为正
    if (x >= left && x <= right && y >= top && y <= bottom)
        return true;


    // 4. 检查矩形四边与圆弧是否有交点
    // 矩形四边：上下左右
    struct Line { std::pair<QPointF, QPointF> p; };
    Line edges[4] = {
        { {QPointF(left, top), QPointF(right, top)} },    // 上
        { {QPointF(right, top), QPointF(right, bottom)} },// 右
        { {QPointF(right, bottom), QPointF(left, bottom)} },// 下
        { {QPointF(left, bottom), QPointF(left, top)} }   // 左
    };

    for (int i = 0; i < 4; ++i) {
        auto inter = linecircleintersection(edges[i].p,getTwoPoints());
        if(!inter.exist){
            continue;
        }
        if(0<=inter.t[0] && inter.t[0]<=1 && thetainst(Theta(inter.p[0]-position()),Angles_.first,Angles_.second)){
            return true;
        }
        if(0<=inter.t[1] && inter.t[1]<=1 && thetainst(Theta(inter.p[1]-position()),Angles_.first,Angles_.second)){
            return true;
        }
    }
    return false;
}


TwoPointCircleCreator::TwoPointCircleCreator(){
    inputType.push_back({ObjectType::Point, ObjectType::Point});
    operationName = "TwoPointCircleCreator";
    waitImplemented = true;
}

std::set<GeometricObject*> TwoPointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                         QPointF position) const {
    return std::set<GeometricObject*>{(new Circle(objs,0))->flush()};
}

std::set<GeometricObject*> TwoPointCircleCreator::wait(std::vector<GeometricObject*> objs) const {
    return {new Circle(objs, 0, true)};
}

CenterRadiusCircleCreator::CenterRadiusCircleCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Lineoo});
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "CenterRadiusCircleCreator";
}

std::set<GeometricObject*> CenterRadiusCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                             QPointF position) const {
    return std::set<GeometricObject*>{(new Circle(objs,1))->flush()};
}

ThreePointCircleCreator::ThreePointCircleCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "ThreePointCircleCreator";
    waitImplemented = true;
}

std::set<GeometricObject*> ThreePointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                           QPointF position) const {
    return std::set<GeometricObject*>{(new Circle(objs,2))->flush()};
}

std::set<GeometricObject*> ThreePointCircleCreator::wait(std::vector<GeometricObject*> objs) const {
    return {new Circle(objs, 2, true)};
}

SemicircleCreator::SemicircleCreator(){
    inputType.push_back({ObjectType::Point, ObjectType::Point});
    operationName = "SemiircleCreator";
    waitImplemented = true;
}

std::set<GeometricObject*> SemicircleCreator::apply(std::vector<GeometricObject*> objs,
                                                           QPointF position) const {
    return std::set<GeometricObject*>{(new Arc(objs,0))->flush()};
}

std::set<GeometricObject*> SemicircleCreator::wait(std::vector<GeometricObject*> objs) const {
    return { (new Arc(objs, 0, true)) };
}

CenterTwoPointArcCreator::CenterTwoPointArcCreator(){
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "CenterTwoPointArcCreator";
    waitImplemented = true;
}

std::set<GeometricObject*> CenterTwoPointArcCreator::apply(std::vector<GeometricObject*> objs,
                                                     QPointF position) const {
    return std::set<GeometricObject*>{(new Arc(objs,1))->flush()};
}

std::set<GeometricObject*> CenterTwoPointArcCreator::wait(std::vector<GeometricObject*> objs) const {
    return { (new Arc(objs, 1, true)) };
}
