#ifndef CIRCLE_H
#define CIRCLE_H

#include "geometricobject.h"
#include "point.h" // 如果圆心是一个Point对象，或者你需要Point来定义圆
#include <QPointF>
#include <cmath> // For std::sqrt in isNear
#include "operation.h"

class Saveloadhelper;

class Circle : public GeometricObject {
public:
    // 构造函数: 可以有多种，例如通过圆心点对象和半径，或通过圆心坐标和半径
    explicit Circle(const std::vector<GeometricObject*>& parents, const int& generation,
                    bool isTemp = false, bool aux = false);

    // --- 重写 GeometricObject 的纯虚函数 ---
    ObjectType getObjectType() const override { return ObjectType::Circle; } // 假设ObjectType::Circle存在
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& pos) const override;
    QPointF position() const override; // 通常返回圆心

    long double getRadius() const;
    GeometricObject* flush() override;
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const override;

    std::pair<const QPointF, const QPointF> getTwoPoints() const override;

protected:

    Qt::PenStyle getPenStyle() const;

};

//Circle 的产生方式:
//0圆心+圆周上一点 1:圆心+线段 2:三点

class TwoPointCircleCreator: public Operation {
public:
    TwoPointCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
    std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const override;
};

class CenterRadiusCircleCreator : public Operation {
public:
    CenterRadiusCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class ThreePointCircleCreator : public Operation {
public:
    ThreePointCircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
    std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const override;
};



class Arc : public GeometricObject {
public:
    // 构造函数: 可以有多种，例如通过圆心点对象和半径，或通过圆心坐标和半径
    explicit Arc(const std::vector<GeometricObject*>& parents, const int& generation,
                 bool isTemp = false, bool aux = false);

    // --- 重写 GeometricObject 的纯虚函数 ---
    ObjectType getObjectType() const override { return ObjectType::Arc; } // 假设ObjectType::Circle存在
    void draw(QPainter* painter) const override;
    bool isNear(const QPointF& pos) const override;
    QPointF position() const override; // 返回圆心

    long double getRadius() const;
    GeometricObject* flush() override;
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const override;

    std::pair<const QPointF, const QPointF> getTwoPoints() const override;//Arc的getTwoPoints保证second是弧的起点

    std::pair<long double, long double> getAngles() const;

    friend class Saveloadhelper;
protected:
    std::pair<long double,long double> Angles_;
    Qt::PenStyle getPenStyle() const;

};
//Arc产生方式
//0:两点半圆, 1:弧心+弧起点+弧终点方向, 2:过三点的弧

class SemicircleCreator : public Operation {
public:
    SemicircleCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
    std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const override;
};

class CenterTwoPointArcCreator : public Operation {
public:
    CenterTwoPointArcCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
    std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const override;
};

#endif // CIRCLE_H
