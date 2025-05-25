#include "circle.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <cmath> // For std::pow, std::sqrt
#include <QMessageBox>

// 初始化全局默认值 (如果需要，这些通常在主程序或特定初始化函数中完成，
// 但这里作为示例，假设你需要在objecttype.cpp或类似地方添加Circle的默认值)
// extern std::map<ObjectType, QString> GetDefaultLable;
// extern std::map<ObjectType, QColor> GetDefaultColor;
// extern std::map<ObjectType, double> GetDefaultSize;
// extern std::map<ObjectType, int> GetDefaultShape;
// ... 你可能需要在某处初始化这些map中关于Circle的条目 ...

Circle::Circle(Point* centerPoint, double radius, ObjectName name)
    : GeometricObject(name),
    centerPoint_(centerPoint),
    pointOnCircle_(nullptr),
    centerPosition_(),
    radius_(radius > 0 ? radius : 10.0),
    circleType_(CircleType::FULL_CIRCLE),
    startAnglePoint_(),
    endAnglePoint_(),
    startAngle_(0.0),
    spanAngle_(360.0) {
    if (centerPoint_) {
        this->addParent(centerPoint_);
        updateCenterPositionFromPoint();
    }
}



Circle::Circle(const QPointF& centerPos, double radius, ObjectName name)
    : GeometricObject(name), centerPoint_(nullptr), pointOnCircle_(nullptr), centerPosition_(centerPos), radius_(radius > 0 ? radius : 10.0) {
    // 同上，设置颜色、标签等
}

Circle::Circle(Point* centerPoint, Point* pointOnCircle)
    : GeometricObject(ObjectName::Circle), centerPoint_(centerPoint), pointOnCircle_(pointOnCircle), centerPosition_() {
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
    if (isHidden()) return;

    // 获取圆心和半径
    auto points = getTwoPoints();
    QPointF center = points.first;
    double radius = QLineF(points.first, points.second).length();

    // 设置画笔
    QPen pen;
    pen.setColor(getColor());
    pen.setWidthF(getSize());
    painter->setPen(pen);

    // 根据圆的类型绘制不同形状
    switch (circleType_) {
    case CircleType::FULL_CIRCLE:
        painter->drawEllipse(center, radius, radius);
        break;
    case CircleType::SEMICIRCLE:
        // 绘制半圆 - 使用startAngle_和spanAngle_
        {
            QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
            painter->drawArc(rect, static_cast<int>(-startAngle_ * 16), static_cast<int>(-spanAngle_ * 16));
            break;
        }
    case CircleType::ARC:
        // 绘制圆弧 - 使用startAngle_和spanAngle_
        // Qt中角度是以1/16度为单位的，所以需要乘以16
        {
            QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
            painter->drawArc(rect, static_cast<int>(-startAngle_ * 16), static_cast<int>(-spanAngle_ * 16));
            break;
        }
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

Point* Circle::getCenterPoint() const {
    return centerPoint_;
}

QPointF Circle::getCenterCoordinates() const {
    return getTwoPoints().first;
}

double Circle::getRadius() const {
    auto points = getTwoPoints();
    return QLineF(points.first, points.second).length();
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

    QPointF center;
    QPointF radiusPoint;

    // 根据圆的定义方式获取圆心和半径点
    if (centerPoint_) {
        center = centerPoint_->position();

        if (pointOnCircle_) {
            // 如果有圆上的点，使用它计算半径点
            radiusPoint = pointOnCircle_->position();
        } else {
            // 如果只有圆心和半径值，计算一个半径点（例如向右radius_单位）
            radiusPoint = QPointF(center.x() + radius_, center.y());
        }
    } else {
        // 如果没有centerPoint_，使用存储的centerPosition_
        center = centerPosition_;
        radiusPoint = QPointF(center.x() + radius_, center.y());
    }

    return std::make_pair(center, radiusPoint);
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


// 1. 圆心和半径作圆
CenterRadiusCircleCreator::CenterRadiusCircleCreator() {
    inputType.push_back({ObjectType::Point}); // 只需要一个点作为圆心
    operationName = "CenterRadiusCircleCreator";
}

std::set<GeometricObject*> CenterRadiusCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                             QPointF position) const {
    std::set<GeometricObject*> s;
    if (objs.size() < 1) return s;

    Point* center = dynamic_cast<Point*>(objs[0]);
    if (!center) return s;

    // 使用position作为半径点
    double radius = QLineF(center->position(), position).length();
    Circle* circle = new Circle(center, radius);
    circle->setCircleType(CircleType::FULL_CIRCLE);
    s.insert(circle);
    return s;
}

// 2. 三点作圆
ThreePointCircleCreator::ThreePointCircleCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "ThreePointCircleCreator";
}

std::set<GeometricObject*> ThreePointCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                           QPointF position) const {
    std::set<GeometricObject*> s;
    if (objs.size() < 3) return s;

    Point* p1 = dynamic_cast<Point*>(objs[0]);
    Point* p2 = dynamic_cast<Point*>(objs[1]);
    Point* p3 = dynamic_cast<Point*>(objs[2]);

    if (!p1 || !p2 || !p3) return s;

    // 计算三点确定的圆的圆心和半径
    QPointF pos1 = p1->position();
    QPointF pos2 = p2->position();
    QPointF pos3 = p3->position();

    // 使用三点确定圆的公式计算圆心
    double A = pos1.x() * (pos2.y() - pos3.y()) - pos1.y() * (pos2.x() - pos3.x()) + pos2.x() * pos3.y() - pos3.x() * pos2.y();
    double B = (pos1.x() * pos1.x() + pos1.y() * pos1.y()) * (pos3.y() - pos2.y()) +
               (pos2.x() * pos2.x() + pos2.y() * pos2.y()) * (pos1.y() - pos3.y()) +
               (pos3.x() * pos3.x() + pos3.y() * pos3.y()) * (pos2.y() - pos1.y());
    double C = (pos1.x() * pos1.x() + pos1.y() * pos1.y()) * (pos2.x() - pos3.x()) +
               (pos2.x() * pos2.x() + pos2.y() * pos2.y()) * (pos3.x() - pos1.x()) +
               (pos3.x() * pos3.x() + pos3.y() * pos3.y()) * (pos1.x() - pos2.x());

    // 检查三点是否共线（A接近0）
    if (std::abs(A) < 1e-10) return s;

    double centerX = -B / (2 * A);
    double centerY = -C / (2 * A);
    QPointF center(centerX, centerY);

    // 计算半径
    double radius = QLineF(center, pos1).length();

    // 创建圆
    Point* centerPoint = new Point(center);
    Circle* circle = new Circle(centerPoint, radius);
    circle->setCircleType(CircleType::FULL_CIRCLE);

    // 添加依赖关系
    circle->addParent(p1);
    circle->addParent(p2);
    circle->addParent(p3);

    s.insert(centerPoint);
    s.insert(circle);
    return s;
}

// 3. 圆弧
ArcCreator::ArcCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "ArcCreator";
}
std::set<GeometricObject*> ArcCreator::apply(std::vector<GeometricObject*> objs,
                                              QPointF position) const {
    std::set<GeometricObject*> s;
    if (objs.size() < 3) return s;

    Point* center = dynamic_cast<Point*>(objs[0]);
    Point* startPoint = dynamic_cast<Point*>(objs[1]);
    Point* endPoint = dynamic_cast<Point*>(objs[2]);

    if (!center || !startPoint || !endPoint) return s;

    // 使用圆心和起始点的距离作为半径
    double radius = QLineF(center->position(), startPoint->position()).length();

    // 创建圆弧
    Circle* arc = new Circle(center, radius);
    arc->setCircleType(CircleType::ARC);

    // 直接使用Qt的角度系统计算角度
    QLineF startLine(center->position(), startPoint->position());
    double startAngle = startLine.angle();

    QLineF endLine(center->position(), endPoint->position());
    double endAngle = endLine.angle();

    // 确保结束角度大于起始角度（顺时针方向）
    if (endAngle < startAngle) {
        endAngle += 360.0;
    }

    double spanAngle = endAngle - startAngle;

    arc->setArcAngles(startAngle, spanAngle);

    // 添加依赖关系
    arc->addParent(startPoint);
    arc->addParent(endPoint);

    s.insert(arc);
    return s;
}


// 4. 半圆
SemicircleCreator::SemicircleCreator() {
    inputType.push_back({ObjectType::Point, ObjectType::Point});
    operationName = "SemicircleCreator";
}
std::set<GeometricObject*> SemicircleCreator::apply(std::vector<GeometricObject*> objs,
                                                     QPointF position) const {
    std::set<GeometricObject*> s;
    if (objs.size() < 2) return s;  // 只需要两个点：直径的两端

    Point* pointA = dynamic_cast<Point*>(objs[0]);
    Point* pointB = dynamic_cast<Point*>(objs[1]);

    if (!pointA || !pointB) return s;

    // 获取点的位置
    QPointF posA = pointA->position();
    QPointF posB = pointB->position();

    // 计算圆心 - 两点的中点
    QPointF center((posA.x() + posB.x()) / 2.0, (posA.y() + posB.y()) / 2.0);

    // 创建圆心点
    Point* centerPoint = new Point(center);

    // 计算半径 - 中点到任一点的距离
    double radius = QLineF(center, posA).length();

    // 创建半圆
    Circle* semicircle = new Circle(centerPoint, radius);
    semicircle->setCircleType(CircleType::SEMICIRCLE);

    // 计算半圆的方向 - 从A到B的向量垂直方向
    double dx = posB.x() - posA.x();
    double dy = posB.y() - posA.y();

    // 计算从A到B的角度
    double angle = std::atan2(dy, dx) * 180.0 / M_PI;

    // 设置半圆的起始角度为垂直于AB的方向，扫过180度
    semicircle->setArcAngles(angle, 180.0);

    // 添加依赖关系
    semicircle->addParent(pointA);
    semicircle->addParent(pointB);
    s.insert(centerPoint);
    s.insert(semicircle);

    return s;
}

