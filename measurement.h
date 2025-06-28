#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "geometricobject.h" // 已更新以匹配提供的文件名
#include "operation.h"
#include "point.h"
#include <QPointF>
#include <QColor>
#include <QPen>   // 用于 draw 方法中的 QPen
#include <cmath>  // 用于 isNear 方法中的 std::sqrt, std::fabs
#include <QMessageBox>
#include "calculator.h"

extern int NumOfMeasurements;

class Saveloadhelper;

class Measurement : public GeometricObject {
public:
    // 构造函数
    explicit Measurement(const std::vector<GeometricObject*>& parents, const int& generation);


    // 重写 GeometricObject 中的纯虚函数
    ObjectType getObjectType() const override{return ObjectType::Measurement;}
    void draw(QPainter* painter) const override; // 绘制函数
    bool isNear(const QPointF& pos) const override; // 判断 点是否在文字附近
    QPointF position() const override; // 返回 左上角
    std::pair<const QPointF, const QPointF> getTwoPoints() const override;//返回 左上角和右下角

    GeometricObject* flush() override;
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const override;

    friend class Saveloadhelper;

protected:
    int id_;//这个标签的序号(决定了其显示的位置)
    QString text_;
};

//生成方式 0:长度, 1:角度

class LengthMeasurementCreator : public Operation{
public:
    LengthMeasurementCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};
class AngleMeasurementCreator : public Operation{
public:
    AngleMeasurementCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

#endif // MEASUREMENT_H
