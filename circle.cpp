#include "circle.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <cmath> // For std::pow, std::sqrt
#include <QMessageBox>
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

Circle::Circle(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Circle){
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    GetDefaultLable[ObjectType::Circle]=nextLineLable(GetDefaultLable[ObjectType::Circle]);
}

void Circle::draw(QPainter* painter) const {
    if (!isShown()) return;

    // 获取圆心和半径
    auto points = getTwoPoints();
    QPointF center = points.first;
    double radius = QLineF(points.first, points.second).length();

    QPen pen;
    double add = ((int)hovered_) * HOVER_ADD_WIDTH;

    // 如果被选中，先绘制一个较宽的选中效果
    if (selected_) {
        QColor selectcolor = getColor().lighter(250);
        selectcolor.setAlpha(128);
        pen.setColor(selectcolor);
        pen.setWidthF(getSize() + add + SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

            painter->drawEllipse(center, radius, radius);
            // 添加标签绘制（如果有）
            if (!labelhidden_) {
                painter->setPen(Qt::black);
                painter->drawText(center.x() + radius + 6,
                                  center.y() - 6,
                                  label_);
            }

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


bool Circle::isNear(const QPointF& pos) const {
    if (isHidden()) return false;

    // 使用 getTwoPoints 获取圆心和半径点
    auto points = getTwoPoints();
    QPointF currentCenter = points.first;
    double currentRadius = QLineF(points.first, points.second).length();

    double distToCenter = std::sqrt(std::pow(pos.x() - currentCenter.x(), 2) +
                                    std::pow(pos.y() - currentCenter.y(), 2));

    // 容差考虑了线宽和一些额外像素
    double tolerance = getSize() / 2.0 + 2.0;
    return std::abs(distToCenter - currentRadius) <= tolerance;
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
        position_.push_back(parents_[0]->position()),position_.push_back(parents_[0]->position()+QPointF(dynamic_cast<Lineoo*>(parents_[1])->length(),0));
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

QPointF Circle::position() const {
    return getTwoPoints().first;
}

double Circle::getRadius() const {
    const auto& points = getTwoPoints();
    return len(points.first - points.second);
}

std::pair<const QPointF, const QPointF> Circle::getTwoPoints() const {
    return std::make_pair(position_[0],position_[1]);
}

TwoPointCircleCreator::TwoPointCircleCreator(){
    inputType.push_back({ObjectType::Point, ObjectType::Point});
    operationName = "TwoPointCircleCreator";
}

std::set<GeometricObject*> TwoPointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                         QPointF position) const {
    return std::set<GeometricObject*>{(new Circle(objs,0))->flush()};
}

CenterRadiusCircleCreator::CenterRadiusCircleCreator() {
    // 需要三个点：前两点距离作为半径，第三点为圆心
    inputType.push_back({ObjectType::Point, ObjectType::Lineoo});
    operationName = "CenterRadiusCircleCreator";
}

std::set<GeometricObject*> CenterRadiusCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                             QPointF position) const {
    return std::set<GeometricObject*>{(new Circle(objs,1))->flush()};
}

ThreePointCircleCreator::ThreePointCircleCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "ThreePointCircleCreator";
}

std::set<GeometricObject*> ThreePointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                           QPointF position) const {
    return std::set<GeometricObject*>{(new Circle(objs,2))->flush()};
}

bool Circle::isTouchedByRectangle(const QPointF& start, const QPointF& end) const {
    auto p = getTwoPoints();
    double dist = QLineF(p.first, p.second).length();
    double x = p.first.x(), y = p.first.y();
    std::vector<double> v = {start.x(), end.y(), end.x(), start.y()};
        std::vector<double> w = {x, y};
        double minDist = 1e9, maxDist = 0;
        for (int i = 0; i < 4; ++i){
                maxDist = std::max(QLineF(QPointF(v[2 * (i/2)], v[2 * (i%2) + 1]), p.first).length(), maxDist);
            }
        double minX = std::min(std::abs(start.x() - x), std::abs(end.x() - x));
        double minY = std::min(std::abs(start.y() - y), std::abs(end.y() - y));
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
