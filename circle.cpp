#include "circle.h"
#include <QPen>
#include <QBrush>
#include <cmath> // For std::pow, std::sqrt

// 初始化全局默认值 (如果需要，这些通常在主程序或特定初始化函数中完成，
// 但这里作为示例，假设你需要在objecttype.cpp或类似地方添加Circle的默认值)
// extern std::map<ObjectType, QString> GetDefaultLable;
// extern std::map<ObjectType, QColor> GetDefaultColor;
// extern std::map<ObjectType, double> GetDefaultSize;
// extern std::map<ObjectType, int> GetDefaultShape;
// ... 你可能需要在某处初始化这些map中关于Circle的条目 ...

Circle::Circle(Point* centerPoint, double radius, ObjectName name)
    : GeometricObject(name), centerPoint_(centerPoint), centerPosition_(), radius_(radius > 0 ? radius : 10.0) { // 半径至少为正
    if (centerPoint_) {
        this->addParent(centerPoint_); // 将圆心点设为父对象
        updateCenterPositionFromPoint();
    }
    // 从全局默认值或特定逻辑设置颜色、标签等
    // color_ = GetDefaultColor[getObjectType()]; // 示例
    // label_ = GetDefaultLable[getObjectType()]; // 示例
    // size_ = GetDefaultSize[getObjectType()];   // 示例 (可能代表线宽)
}

Circle::Circle(const QPointF& centerPos, double radius, ObjectName name)
    : GeometricObject(name), centerPoint_(nullptr), centerPosition_(centerPos), radius_(radius > 0 ? radius : 10.0) {
    // 同上，设置颜色、标签等
}

Circle::Circle(Point* centerPoint, Point* pointOnCircle)
    : GeometricObject(ObjectType::Circle), centerPoint_(centerPoint), centerPosition_(), pointOnCircle_(pointOnCircle) {
    this->addParent(centerPoint_);
    this->addParent(pointOnCircle_);
    updateCenterPositionFromPoint();
    QPointF pos1 = centerPoint->position(), pos2 = pointOnCircle->position();
    radius_ = std::sqrt(std::pow(pos1.x() - pos2.x(), 2) + std::pow(pos1.y() - pos2.y(), 2));
}

Circle::~Circle() {
    // 如果在构造函数中建立了需要显式断开的连接，在这里处理
    // 例如，如果父子关系不仅仅是添加到列表，还有其他逻辑
    if (centerPoint_) {
        // removeParent(centerPoint_); // 通常GeometricObject的析构函数或父对象的removeChild会处理
    }
}

void Circle::draw(QPainter* painter) const {
    if (!painter || isHidden()) {
        return;
    }

    // ***** 获取当前最准确的圆心坐标 *****
    QPointF currentCenter = getCenterCoordinates(); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 使用此函数获取圆心

    QPen pen(getColor());
    pen.setWidthF(getSize());
    int lineStyle = getShape(); // 获取存储的线型
    if (lineStyle == LineStyle::Dashed) { // 或直接用 1
        pen.setStyle(Qt::DashLine);
    } else if (lineStyle == LineStyle::Dotted) { // 或直接用 2
        pen.setStyle(Qt::DotLine);
    }
    else { // 默认为 LineStyle::Solid 或 0
        pen.setStyle(Qt::SolidLine);
    }

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush); // 圆通常不填充

    double diameter = 2 * radius_;
    // ***** 使用 currentCenter 进行绘制 *****
    painter->drawEllipse(currentCenter.x() - radius_, currentCenter.y() - radius_, diameter, diameter);

    if (!getLabel().isEmpty()) {
        // ***** 根据 currentCenter 调整标签位置 *****
        painter->drawText(currentCenter + QPointF(radius_ / 2, -radius_ / 2), getLabel());
    }
}


bool Circle::isNear(const QPointF& pos) const {
    if (isHidden()) return false;

    // ***** 获取当前最准确的圆心坐标 *****
    QPointF currentCenter = getCenterCoordinates(); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 使用此函数获取圆心

    double distToCenter = std::sqrt(std::pow(pos.x() - currentCenter.x(), 2) +
                                    std::pow(pos.y() - currentCenter.y(), 2));

    // 容差考虑了线宽和一些额外像素
    double tolerance = getSize() / 2.0 + 2.0;
    return std::abs(distToCenter - radius_) <= tolerance;
}


QPointF Circle::position() const {
    // 如果圆心是Point对象且可能移动，确保返回最新的位置
    if (centerPoint_) {
        // ((Circle*)this)->updateCenterPositionFromPoint(); // const_cast if needed, or make updateCenterPositionFromPoint const if it doesn't modify logical state for this call
        return centerPoint_->position(); // 直接返回点的权威位置
    }
    return centerPosition_;
}

Point* Circle::getCenterPoint() const {
    return centerPoint_;
}

QPointF Circle::getCenterCoordinates() const {
    if (centerPoint_) {
        return centerPoint_->position();
    }
    return centerPosition_;
}

double Circle::getRadius() const {
    return radius_;
}

void Circle::setCenterPoint(Point* centerPoint) {
    if (centerPoint_ == centerPoint) return;

    if (centerPoint_) {
        this->removeParent(centerPoint_); // 从旧圆心移除父子关系
    }
    centerPoint_ = centerPoint;
    if (centerPoint_) {
        this->addParent(centerPoint_); // 添加到新圆心
        updateCenterPositionFromPoint();
    } else {
        // 如果新的centerPoint是nullptr，可能需要决定centerPosition_的行为
        // 也许保持不变，或者设置为某个默认值
    }
    // 触发重绘或更新
}

void Circle::setCenterCoordinates(const QPointF& centerPos) {
    if (centerPoint_) {
        // 如果圆心是一个Point对象，理论上应该通过移动那个Point对象来改变圆心位置
        // 直接设置坐标可能会导致与Point对象位置不一致。
        // 除非明确允许圆心坐标独立于Point对象。
        // 这是一个设计决策。
        // 一种可能是：如果设置了centerPoint_，则此函数无效或只更新缓存。
        // 另一种是：此函数会尝试移动centerPoint_（如果存在）。
        // 最简单：如果centerPoint_存在，则忽略此调用或只更新centerPosition_作为缓存。
        centerPoint_->setPosition(centerPos); // 假设Point有setPosition
        updateCenterPositionFromPoint();      // 确保一致
    } else {
        centerPosition_ = centerPos;
    }
    // 触发重绘或更新
}

void Circle::setRadius(double radius) {
    if (radius > 0) {
        radius_ = radius;
        // 触发重绘或更新
    }
}

void Circle::updateCenterPositionFromPoint() {
    if (centerPoint_) {
        centerPosition_ = centerPoint_->position();
    }
}

TwoPointCircleCreator::TwoPointCircleCreator(){
    inputType.push_back({ObjectType::Point, ObjectType::Point});
    operationName = "TwoPointCircleCreator";
}

std::set<GeometricObject*> TwoPointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                 QPointF position) const {
    std::set<GeometricObject*> s;
    Point* p1 = dynamic_cast<Point*>(objs[0]);
    Point* p2 = dynamic_cast<Point*>(objs[1]);
    s.insert(new Circle(p1, p2));
    return s;
}
