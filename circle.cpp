#include "circle.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <cmath> // For std::pow, std::sqrt
#include <QMessageBox>
#include "lineoo.h"
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

void Circle::setLineStyle(int style) {
    shape_ = style;
    // 可以在这里添加更新/重绘的通知
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

    // 根据 hovered 状态增加线宽
    double add = ((int)hovered_) * HOVER_ADD_WIDTH;

    // 如果被选中，先绘制一个较宽的选中效果
    if (selected_) {
        QColor selectcolor = getColor().lighter(250);
        selectcolor.setAlpha(128);
        pen.setColor(selectcolor);
        pen.setWidthF(getSize() + add + SELECTED_WIDTH);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        // 根据圆的类型绘制选中效果
        switch (circleType_) {
        case CircleType::FULL_CIRCLE:
            painter->drawEllipse(center, radius, radius);
            // 添加标签绘制（如果有）
            if (label_ != "") {
                painter->setPen(Qt::black);
                painter->drawText(center.x() + radius + 6,
                                  center.y() - 6,
                                  label_);
            }
            break;
        case CircleType::SEMICIRCLE:
        {
            QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
            painter->drawArc(rect, static_cast<int>(-startAngle_ * 16), static_cast<int>(-spanAngle_ * 16));
            break;
        }
        case CircleType::ARC:
        {
            QRectF rect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
            painter->drawArc(rect, static_cast<int>(startAngle_ * 16), static_cast<int>(spanAngle_ * 16));
            break;
        }
        }
    }

    // 设置正常绘制的画笔
    pen.setColor(getColor());
    pen.setWidthF(getSize() + add); // 添加 hovered 状态的额外宽度
    pen.setStyle(getPenStyle());    // 假设您有 getPenStyle() 方法
    painter->setPen(pen);

    // 根据圆的类型绘制不同形状
    switch (circleType_) {
    case CircleType::FULL_CIRCLE:
        painter->drawEllipse(center, radius, radius);
        // 添加标签绘制（如果有）
        if (label_ != "") {
            painter->setPen(Qt::black);
            painter->drawText(center.x() + radius + 6,
                              center.y() - 6,
                              label_);
        }
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
            painter->drawArc(rect, static_cast<int>(startAngle_ * 16), static_cast<int>(spanAngle_ * 16));
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

    // 三点作圆特殊处理
    if (threePointMode_ && threePoints_[0] && threePoints_[1] && threePoints_[2]) {
        QPointF pos1 = threePoints_[0]->position();
        QPointF pos2 = threePoints_[1]->position();
        QPointF pos3 = threePoints_[2]->position();

        // 使用三点确定圆的公式计算圆心
        double A = pos1.x() * (pos2.y() - pos3.y()) - pos1.y() * (pos2.x() - pos3.x()) + pos2.x() * pos3.y() - pos3.x() * pos2.y();

        // 检查三点是否共线
        if (std::abs(A) < 1e-10) {
            legal_ = false;
            return std::make_pair(QPointF(), QPointF(1, 1));
        }

        double B = (pos1.x() * pos1.x() + pos1.y() * pos1.y()) * (pos3.y() - pos2.y()) +
                   (pos2.x() * pos2.x() + pos2.y() * pos2.y()) * (pos1.y() - pos3.y()) +
                   (pos3.x() * pos3.x() + pos3.y() * pos3.y()) * (pos2.y() - pos1.y());
        double C = (pos1.x() * pos1.x() + pos1.y() * pos1.y()) * (pos2.x() - pos3.x()) +
                   (pos2.x() * pos2.x() + pos2.y() * pos2.y()) * (pos3.x() - pos1.x()) +
                   (pos3.x() * pos3.x() + pos3.y() * pos3.y()) * (pos1.x() - pos2.x());

        QPointF calculatedCenter(-B / (2 * A), -C / (2 * A));
        double calculatedRadius = QLineF(calculatedCenter, pos1).length();

        return std::make_pair(calculatedCenter, QPointF(calculatedCenter.x() + calculatedRadius, calculatedCenter.y()));
    }
    //点加半径情况
    else if (parents_.size() == 3) {
        // 尝试获取三个父对象作为点
        Point* p1 = dynamic_cast<Point*>(parents_[2]);
        Point* p2 = dynamic_cast<Point*>(parents_[1]);
        Point* center = dynamic_cast<Point*>(parents_[0]);

        if (p1 && p2 && center) {
            // 计算半径
            double dynamicRadius = QLineF(p1->position(), p2->position()).length();
            // 返回圆心和半径点
            QPointF centerPos = center->position();
            return std::make_pair(centerPos, QPointF(centerPos.x() + dynamicRadius, centerPos.y()));
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
    // 需要三个点：前两点距离作为半径，第三点为圆心
    inputType.push_back({ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "CenterRadiusCircleCreator";
}



std::set<GeometricObject*> CenterRadiusCircleCreator::apply(std::vector<GeometricObject*> objs,
                                                             QPointF position) const {
    std::set<GeometricObject*> s;

    if (objs.size() < 3) {
        qDebug() << "对象数量不足3个，返回空集";
        return s;
    }

    Point* p1 = dynamic_cast<Point*>(objs[0]);
    Point* p2 = dynamic_cast<Point*>(objs[1]);
    Point* center = dynamic_cast<Point*>(objs[2]);
    if (!p1 || !p2 || !center) {
        qDebug() << "类型转换失败，返回空集";
        return s;
    }

    // 计算半径
    double radius = QLineF(p1->position(), p2->position()).length();
    qDebug() << "计算半径:" << radius;

    // 创建圆
    Circle* circle = new Circle(center, radius);
    circle->setCircleType(CircleType::FULL_CIRCLE);

    circle->addParent(p1);     // 添加第一个点作为父对象
    circle->addParent(p2);     // 添加第二个点作为父对象
    circle->addParent(center); // 这个已经在Circle构造函数中添加了
    qDebug() << "创建圆对象:" << circle;
    s.insert(circle);

    return s;
}





// 2. 三点作圆
void Circle::setThreePointMode(Point* p1, Point* p2, Point* p3) {
    threePointMode_ = true;
    threePoints_[0] = p1;
    threePoints_[1] = p2;
    threePoints_[2] = p3;

    // 添加父子关系
    this->addParent(p1);
    this->addParent(p2);
    this->addParent(p3);
}

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

    // 创建一个临时圆心点（可以放在任意位置，因为会被动态计算覆盖）
    Point* tempCenter = new Point(QPointF(0, 0));

    // 创建圆
    Circle* circle = new Circle(tempCenter, 1.0);  // 半径也会被动态计算覆盖
    circle->setCircleType(CircleType::FULL_CIRCLE);

    // 设置为三点模式
    circle->setThreePointMode(p1, p2, p3);

    s.insert(tempCenter);
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

    // 计算起始点和终点的角度
    QLineF startLine(center->position(), startPoint->position());
    double startAngle = startLine.angle();

    QLineF endLine(center->position(), endPoint->position());
    double endAngle = endLine.angle();

    // 计算角度差（确保在0-360度范围内）
    double angleDiff = std::fmod(endAngle - startAngle + 360.0, 360.0);

    // 计算逆时针方向的角度差
    double ccwAngleDiff = std::fmod(startAngle - endAngle + 360.0, 360.0);

    // 选择较小的角度差（小于180度的弧）
    double spanAngle;
    if (angleDiff <= 180.0) {
        // 顺时针方向的弧小于180度
        spanAngle = angleDiff;
    } else {
        // 逆时针方向的弧小于180度
        // 在Qt中，负的spanAngle表示逆时针方向
        spanAngle = -ccwAngleDiff;
    }

    arc->setArcAngles(startAngle, spanAngle);

    // 添加依赖关系
    arc->addParent(center);  // 添加圆心作为依赖
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

