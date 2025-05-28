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

QPointF Circle::position() const {
    // 返回圆心位置
    return getTwoPoints().first;
}

double Circle::getRadius() const {
    const auto& points = getTwoPoints();
    return len(points.first - points.second);
}

std::pair<const QPointF, const QPointF> Circle::getTwoPoints() const {
    // 检查父对象的合法性
    legal_ = true;
    for (auto iter : parents_) {
        if (!iter->isLegal()) {
            legal_ = false;
            return std::make_pair(QPointF(), QPointF(1, 1));
        }
    }

    switch(generation_){
    case 0:{
        return std::make_pair(parents_[0]->position(),parents_[1]->position());
    }
    case 1:{
        return std::make_pair(parents_[0]->position(),parents_[0]->position()+QPointF(dynamic_cast<Lineoo*>(parents_[1])->length(),0));
    }
    case 2:{
        return std::make_pair(calculateCircleCenter(parents_[0]->position(),parents_[1]->position(),parents_[2]->position()),parents_[0]->position());
    }
    default:{
        return std::make_pair(QPointF(0,0),QPointF(1,1));
    }
    }
}

TwoPointCircleCreator::TwoPointCircleCreator(){
    inputType.push_back({ObjectType::Point, ObjectType::Point});
    operationName = "TwoPointCircleCreator";
}

std::set<GeometricObject*> TwoPointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                         QPointF position) const {
    return std::set<GeometricObject*>{new Circle(objs,0)};
}

CenterRadiusCircleCreator::CenterRadiusCircleCreator() {
    // 需要三个点：前两点距离作为半径，第三点为圆心
    inputType.push_back({ObjectType::Point, ObjectType::Lineoo});
    operationName = "CenterRadiusCircleCreator";
}

std::set<GeometricObject*> CenterRadiusCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                             QPointF position) const {
    return std::set<GeometricObject*>{new Circle(objs,1)};
}

ThreePointCircleCreator::ThreePointCircleCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "ThreePointCircleCreator";
}

std::set<GeometricObject*> ThreePointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                           QPointF position) const {
    return std::set<GeometricObject*>{new Circle(objs,2)};
}
