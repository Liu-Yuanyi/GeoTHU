#ifndef GEOMETRICOBJECT_H
#define GEOMETRICOBJECT_H

#include <QPainter>
#include <vector>
#include "objecttype.h"

class GeometricObject {
public:
    ObjectName name;

    virtual ~GeometricObject() = default;
    virtual void draw(QPainter* painter) const = 0;
    bool isSelected() const { return selected_; }
    bool isHidden() const { return hidden_; }
    bool islegal() const { return legal_; }
    void setHidden(bool hidden) { hidden_ = hidden; }
    void setSelected(bool selected) { selected_ = selected;}
    void setHovered(bool hovered) {hovered_ = hovered;}
    virtual void setColor(QColor color) = 0;
    virtual void setSize(double size) = 0;
    virtual bool isNear(const QPointF& Pos) const = 0;
    virtual void setPosition(const QPointF& pos) = 0;
    virtual QPointF position() const = 0;

    GeometricObject(): selected_(true), hovered_(false), legal_(true), hidden_(false) {}

protected:
    bool selected_;
    bool hovered_;
    bool legal_;
    bool hidden_;
    std::vector<GeometricObject*> parents = {};
    std::vector<GeometricObject*> children = {};
};

#endif // GEOMETRICOBJECT_H
