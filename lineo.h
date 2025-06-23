#ifndef LINEO_H
#define LINEO_H

#include "geometricobject.h" // 已更新以匹配提供的文件名
#include "operation.h"
#include "point.h"
#include <QPointF>
#include <QColor>
#include <QPen>   // 用于 draw 方法中的 QPen
#include <cmath>  // 用于 isNear 方法中的 std::sqrt, std::fabs
#include <QMessageBox>

class Lineo : public GeometricObject {
public:
    // 构造函数
    explicit Lineo(const std::vector<GeometricObject*>& parents,const int& generation);

    // 重写 GeometricObject 中的纯虚函数
    ObjectType getObjectType() const override{return ObjectType::Lineo;}
    void draw(QPainter* painter) const override; // 绘制函数
    bool isNear(const QPointF& pos) const override; // 判断点是否在线附近
    QPointF position() const override; // 返回 startPoint_s
    std::pair<const QPointF, const QPointF> getTwoPoints() const override;

    GeometricObject* flush() override;
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const override;

protected:
    // isNear 计算的辅助函数 (点到线段的距离)
    Qt::PenStyle getPenStyle()const;
    long double distanceToLineo(const QPointF& p,const std::pair<QPointF,QPointF>& Points) const;
};

//Lineo的shape:
//0实线, 1虚线, 2点线

//Lineo的生成方式:
//0:两点连线, 1角平分线

class LineoCreator: public Operation{//两点连线
public:
    LineoCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
    std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const override;
};

#endif // LINEO_H
