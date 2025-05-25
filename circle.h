#ifndef CIRCLE_H
#define CIRCLE_H

#include "geometricobject.h"
#include "point.h" // 如果圆心是一个Point对象，或者你需要Point来定义圆
#include <QPointF>
#include <cmath> // For std::sqrt in isNear
#include "operation.h"
enum class CircleType {
    FULL_CIRCLE,      // 完整的圆
    SEMICIRCLE,       // 半圆
    ARC,              // 圆弧
};
class Circle : public GeometricObject {
public:
    // 构造函数: 可以有多种，例如通过圆心点对象和半径，或通过圆心坐标和半径
    explicit Circle(Point* centerPoint, double radius, ObjectName name = ObjectName::Circle); // 假设ObjectName::Circle存在
    explicit Circle(const QPointF& centerPos, double radius, ObjectName name = ObjectName::Circle);
    explicit Circle(Point* centerPoint, Point* pointOnCircle);

    ~Circle() override; // 析构函数

    // --- 重写 GeometricObject 的纯虚函数 ---
    ObjectType getObjectType() const override { return ObjectType::Circle; } // 假设ObjectType::Circle存在
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& pos) const override;
    QPointF position() const override; // 通常返回圆心

    // --- Circle 特有的方法 ---
    Point* getCenterPoint() const; // 如果圆心是Point对象
    QPointF getCenterCoordinates() const; // 获取圆心坐标
    double getRadius() const;

    // 新增：获取圆的两个点（圆心和半径点）
    std::pair<const QPointF, const QPointF> getTwoPoints() const;

    void setCenterPoint(Point* centerPoint); // 如果圆心是Point对象，需要考虑父子关系
    void setCenterCoordinates(const QPointF& centerPos);
    void setRadius(double radius);
    void setCircleType(CircleType type) { circleType_ = type; }
    CircleType getCircleType() const { return circleType_; }
    void setArcAngles(double startAngle, double spanAngle) {
        // 保存角度值（Qt使用的度数）
        startAngle_ = startAngle;
        spanAngle_ = spanAngle;
    }

    void setArcPoints(const QPointF& start, const QPointF& end) {
        startAnglePoint_ = start;
        endAnglePoint_ = end;

        // 计算角度 - 修正版
        QPointF center = getCenterCoordinates();

        // 计算起始角度 (Qt中角度是顺时针的，0度在3点钟方向)
        QLineF line1(center, start);
        double startAngleDegrees = line1.angle(); // Qt角度系统，已经是度数

        // 计算结束角度
        QLineF line2(center, end);
        double endAngleDegrees = line2.angle();

        // 确保结束角度大于起始角度（顺时针方向）
        if (endAngleDegrees < startAngleDegrees) {
            endAngleDegrees += 360.0;
        }

        // 计算跨越角度
        double spanAngleDegrees = endAngleDegrees - startAngleDegrees;

        // 保存角度值
        startAngle_ = startAngleDegrees;
        spanAngle_ = spanAngleDegrees;
    }


private:
    Point* centerPoint_;     // 可选：如果圆心是一个独立的Point对象
    Point* pointOnCircle_;   // 可选：如果圆是通过圆心和圆上一点定义的
    QPointF centerPosition_; // 圆心的坐标 (即使有centerPoint_，也可能缓存其坐标)
    double radius_;
    QPointF startAnglePoint_;  // 用于圆弧和半圆的起始角度点
    QPointF endAnglePoint_;    // 用于圆弧的结束角度点
    double startAngle_ = 0.0;        // 起始角度（弧度）
    double spanAngle_ = 360.0;         // 跨越角度（弧度）
    CircleType circleType_;

    // 辅助函数，用于更新centerPosition_（如果centerPoint_存在）
    void updateCenterPositionFromPoint();
};

class TwoPointCircleCreator: public Operation {
public:
    TwoPointCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const;
};

// 1. 圆心和半径作圆
class CenterRadiusCircleCreator : public Operation {
public:
    CenterRadiusCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const;
};

// 2. 三点作圆
class ThreePointCircleCreator : public Operation {
public:
    ThreePointCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const;
};

// 3. 圆弧
class ArcCreator : public Operation {
public:
    ArcCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const;
};

// 4. 半圆
class SemicircleCreator : public Operation {
public:
    SemicircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const;
};

#endif // CIRCLE_H
