#ifndef GEOMETRICOBJECT_H
#define GEOMETRICOBJECT_H

#include <QPainter>
#include <vector>
#include "objecttype.h"

extern std::map<ObjectType, QString> GetDefaultLable;
extern std::map<ObjectType, QColor> GetDefaultColor;
extern std::map<ObjectType, double> GetDefaultSize;
extern std::map<ObjectType, int> GetDefaultShape;

class GeometricObject {
public:

    GeometricObject(ObjectName name);

    virtual ~GeometricObject();

    virtual ObjectType getObjectType() const = 0;
    virtual void draw(QPainter* painter) const = 0;
    virtual bool isNear(const QPointF& Pos) const = 0;
    virtual QPointF position() const = 0;

    // --- Status Getters ---
    bool isSelected() const { return selected_; }
    bool isHidden() const { return hidden_; }
    bool isLegal() const { return legal_; }
    bool isHovered() const { return hovered_; }
    QString getLabel() const { return label_; }
    QColor getColor() const { return color_; }
    double getSize() const { return size_; }
    int getShape() const { return shape_; }

    // --- Status Setters ---
    void setSelected(bool selected) { selected_ = selected; }
    void setHidden(bool hidden) { hidden_ = hidden; }
    void setLegal(bool legal) { legal_ = legal ;}
    void setHovered(bool hovered) { hovered_ = hovered; }
    void setLabel(QString str) { label_ = str; } // update default label to the next char maybe?
    void setColor(QColor color){ color_ = color; GetDefaultColor[name_] = color; }
    void setSize(double size) { size_ = size; GetDefaultSize[name_] = size; }
    void setShape(int shape) { shape_ = shape; GetDefaultShape[name_] = shape;}

    // --- Parent Management ---
    bool addParent(GeometricObject* parent);//四个add/remove函数都内置了双向设置, 也就是只要A设置add/remove B, B自动就会add/remove A
    bool removeParent(GeometricObject* parent);
    const std::vector<GeometricObject*>& getParents() const;
    bool hasParent(GeometricObject* parent) const;

    // --- Child Management ---
    bool addChild(GeometricObject* child);
    bool removeChild(GeometricObject* child);
    const std::vector<GeometricObject*>& getChildren() const;
    bool hasChild(GeometricObject* child) const;

protected:
    bool selected_;
    bool hovered_;
    bool legal_;
    bool hidden_;
    std::vector<GeometricObject*> parents_ = {};
    std::vector<GeometricObject*> children_ = {};
    QString label_;
    QColor color_;
    double size_;
    int shape_;
    int generation_;//这个对象是怎么产生的
    ObjectName name_;
};

#endif // GEOMETRICOBJECT_H
