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

    GeometricObject(
        const QString& label,
        const QColor& color,
        const double& size,
        const int& shape,
        const int& generatingmethod=0):
        selected_(true), hovered_(false), legal_(true), hidden_(false),
        label_(label), color_(color), size_(size), shape_(shape), generation_(generatingmethod){}

    virtual ~GeometricObject() {
        // 移除父子关系
        std::vector<GeometricObject*> parents_copy = parents_;
        for (GeometricObject* p : parents_copy) {
            if (p) {
                p->removeChild(this);
            }
        }

        std::vector<GeometricObject*> children_copy = children_;
        for (GeometricObject* c : children_copy) {
            if (c) {
                c->removeParent(this);
            }
        }

        // 删除所有子对象
        for (GeometricObject* c : children_) {
            delete c;  // 删除子对象
        }
        children_.clear();  // 清空子对象列表
        parents_.clear();
    }

    virtual ObjectType getObjectType()const = 0;
    virtual void draw(QPainter* painter) const = 0;
    virtual bool isNear(const QPointF& Pos) const = 0;
    virtual QPointF position() const = 0;

    // Status Getters
    bool isSelected() const { return selected_; }
    bool isHidden() const { return hidden_; }
    bool isLegal() const { return legal_; }
    bool isHovered() const { return hovered_; }
    QString getLabel() const { return label_; }
    QColor getColor() const { return color_; }
    double getSize() const { return size_; }
    int getShape() const { return shape_; }

    // Status Setters
    void setSelected(bool selected) { selected_ = selected;}
    void setHidden(bool hidden) { hidden_ = hidden; }
    void setLegal(bool legal) { legal_ = legal ;}
    void setHovered(bool hovered) {hovered_ = hovered;}
    void setLabel(QString str) { label_ = str; }
    void setColor(QColor color){ color_ = color;}
    void setSize(double size) {size_ = size;}
    void setShape(int shape) {shape_ = shape;}

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
};

#endif // GEOMETRICOBJECT_H
