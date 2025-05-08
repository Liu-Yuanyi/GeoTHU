#ifndef GEOMETRICOBJECT_CPP
#define GEOMETRICOBJECT_CPP

#include "geometricobject.h"

std::map<ObjectType, QString> GetDefaultLable = {
    {ObjectType::Point, "A"},
    {ObjectType::Line, "l1"},
    {ObjectType::Lineo, "l1"},
    {ObjectType::Lineoo, "l1"},
    {ObjectType::Circle, "Circle1"}
};

std::map<ObjectType, QColor> GetDefaultColor = {
    {ObjectType::Point, Qt::red},
    {ObjectType::Line, Qt::darkBlue},
    {ObjectType::Lineo, Qt::darkBlue},
    {ObjectType::Lineoo, Qt::darkBlue},
    {ObjectType::Circle, Qt::green}
};

std::map<ObjectType, double> GetDefaultSize = {
    {ObjectType::Point, 4.0},
    {ObjectType::Line, 2.0},
    {ObjectType::Lineo, 2.0},
    {ObjectType::Lineoo, 2.0},
    {ObjectType::Circle, 2.0}
};

std::map<ObjectType, int> GetDefaultShape = {
    {ObjectType::Point, 0},
    {ObjectType::Line, 0},
    {ObjectType::Lineo, 0},
    {ObjectType::Lineoo, 0},
    {ObjectType::Circle, 0}
};

bool GeometricObject::addParent(GeometricObject* parent) {
    if (!parent || parent == this) {
        return false; // Invalid operation: null parent or self-parenting
    }
    // Check if already a parent
    if (std::find(parents_.begin(), parents_.end(), parent) != parents_.end()) {
        return false; // Already a parent, no change made
    }
    parents_.push_back(parent);
    parent->addChild(this); // Maintain bidirectional relationship
    return true; // Successfully added to this object's parents list
}

bool GeometricObject::addChild(GeometricObject* child) {
    if (!child || child == this) {
        return false; // Invalid operation: null child or self-child
    }
    // Check if already a child
    if (std::find(children_.begin(), children_.end(), child) != children_.end()) {
        return false; // Already a child, no change made
    }
    children_.push_back(child);
    child->addParent(this); // Maintain bidirectional relationship
    return true; // Successfully added to this object's children list
}

bool GeometricObject::removeChild(GeometricObject* child) {
    if (!child) {
        return false; // Cannot remove nullptr
    }
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
        child->removeParent(this); // Maintain bidirectional relationship
        return true; // Successfully removed from this object's children list
    }
    return false; // Child not found, no change made
}

bool GeometricObject::removeParent(GeometricObject* parent) {
    if (!parent) {
        return false; // Cannot remove nullptr
    }
    auto it = std::find(parents_.begin(), parents_.end(), parent);
    if (it != parents_.end()) {
        parents_.erase(it);
        parent->removeChild(this); // Maintain bidirectional relationship
        return true; // Successfully removed from this object's parents list
    }
    return false; // Parent not found, no change made
}

const std::vector<GeometricObject*>& GeometricObject::getChildren() const {
    return children_;
}

const std::vector<GeometricObject*>& GeometricObject::getParents() const {
    return parents_;
}

bool GeometricObject::hasChild(GeometricObject* child) const {
    if (!child) return false;
    return std::find(children_.begin(), children_.end(), child) != children_.end();
}

bool GeometricObject::hasParent(GeometricObject* parent) const {
    if (!parent) return false;
    return std::find(parents_.begin(), parents_.end(), parent) != parents_.end();
}

#endif
