#ifndef LINEOO_H
#define LINEOO_H

#include "geometricobject.h" // 已更新以匹配提供的文件名
#include "operation.h"
#include "point.h"
#include <QPointF>
#include <QColor>
#include <QPen>   // 用于 draw 方法中的 QPen
#include <cmath>  // 用于 isNear 方法中的 std::sqrt, std::fabs
#include <QMessageBox>
#include "calculator.h"
// isNear 检查用的小容差值 (例如，以像素为单位)
const long double HOVER_ADD_WIDTH =1.5;
const long double SELECTED_WIDTH =2.5;

class Lineoo : public GeometricObject {
public:
    // 构造函数
    explicit Lineoo(const std::vector<GeometricObject*>& parents, const int& generation,
                    bool isTemp = false);

    // 重写 GeometricObject 中的纯虚函数
    ObjectType getObjectType() const override{return ObjectType::Lineoo;}
    void draw(QPainter* painter) const override; // 绘制函数
    bool isNear(const QPointF& pos) const override; // 判断点是否在线附近
    QPointF position() const override; // 返回 startPoint_
    std::pair<const QPointF, const QPointF> getTwoPoints() const override;

    long double length() const{return len(getTwoPoints().first-getTwoPoints().second);}
    GeometricObject* flush() override;
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const override;

protected:
    // isNear 计算的辅助函数 (点到线段的距离)
    Qt::PenStyle getPenStyle()const;
    long double distanceToLineoo(const QPointF& p,const std::pair<QPointF,QPointF>& Points) const;
};

//lineoo的shape:
//0实线, 1虚线, 2点线

//lineoo的生成方式:
//0:两点连线

class LineooCreator: public Operation{ //两点连线
public:
    LineooCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
    std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const override;
};

#endif // LINEOO_H
