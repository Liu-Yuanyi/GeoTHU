#ifndef CIRCLE_H
#define CIRCLE_H

#include "geometricobject.h"
#include "point.h" // 如果圆心是一个Point对象，或者你需要Point来定义圆
#include <QPointF>
#include <cmath> // For std::sqrt in isNear
#include "operation.h"

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

    void setCenterPoint(Point* centerPoint); // 如果圆心是Point对象，需要考虑父子关系
    void setCenterCoordinates(const QPointF& centerPos);
    void setRadius(double radius);

    // 如果圆依赖于一个点对象作为圆心，当这个点移动时，圆也应该更新
    // 这可能通过父子关系和信号槽（如果GeometricObject是QObject）或回调实现
    // 或者在Canvas的拖动逻辑中处理依赖对象的更新

private:
    Point* centerPoint_;     // 可选：如果圆心是一个独立的Point对象
    Point* pointOnCircle_;
    QPointF centerPosition_; // 圆心的坐标 (即使有centerPoint_，也可能缓存其坐标)
    double radius_;

    // 辅助函数，用于更新centerPosition_（如果centerPoint_存在）
    void updateCenterPositionFromPoint();
};

class TwoPointCircleCreator: public Operation {
public:
    TwoPointCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const;
};

#endif // CIRCLE_H
