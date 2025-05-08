#ifndef GEOMETRICOBJECT_H
#define GEOMETRICOBJECT_H

#include <QPainter>
#include <vector>
#include "objecttype.h"

class GeometricObject {
public:
    ObjectName name;

    // Virtual destructor: crucial for proper cleanup of derived classes
    // and for managing parent/child relationships.
    virtual ~GeometricObject() {
        // Notify parents that this object is no longer their child.
        // Iterate over a copy because p->removeChild(this) will modify parents_.
        std::vector<GeometricObject*> parents_copy = parents_;
        for (GeometricObject* p : parents_copy) {
            if (p) { // Ensure pointer is not null
                p->removeChild(this); // This call will also remove p from this->parents_
            }
        }
        // parents_.clear(); // Should be empty now due to reciprocal calls

        // Notify children that this object is no longer their parent.
        // Iterate over a copy because c->removeParent(this) will modify children_.
        std::vector<GeometricObject*> children_copy = children_;
        for (GeometricObject* c : children_copy) {
            if (c) { // Ensure pointer is not null
                c->removeParent(this); // This call will also remove c from this->children_
            }
        }
        // children_.clear(); // Should be empty now due to reciprocal calls
    }

    virtual void draw(QPainter* painter) const = 0;
    virtual void setColor(QColor color) = 0;
    virtual void setSize(double size) = 0;
    virtual bool isNear(const QPointF& Pos) const = 0;
    virtual void setPosition(const QPointF& pos) = 0;
    virtual QPointF position() const = 0;

    // Status Getters
    bool isSelected() const { return selected_; }
    bool isHidden() const { return hidden_; }
    bool islegal() const { return legal_; }
    bool isHovered() const { return hovered_; }
    QString getLabel() const { return label_; }

    // Status Setters
    void setHidden(bool hidden) { hidden_ = hidden; }
    void setSelected(bool selected) { selected_ = selected;}
    void setHovered(bool hovered) {hovered_ = hovered;}
    void setLabel(QString str) { label_ = str; }

    GeometricObject(): selected_(true), hovered_(false), legal_(true), hidden_(false) {}

    // --- Parent Management ---
    bool addParent(GeometricObject* parent) {
        if (!parent || parent == this) {
            return false; // Invalid operation: null parent or self-parenting
        }
        // Check if already a parent
        if (std::find(parents_.begin(), parents_.end(), parent) != parents_.end()) {
            return false; // Already a parent, no change made
        }
        parents_.push_back(parent);
        parent->addChild(this); // Maintain bidirectional relationship
            // addChild will handle its own success/failure internally,
            // and its recursive call to this->addParent will find parent already added.
        return true; // Successfully added to this object's parents list
    }

    bool removeParent(GeometricObject* parent) {
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

    const std::vector<GeometricObject*>& getParents() const {
        return parents_;
    }

    bool hasParent(GeometricObject* parent) const {
        if (!parent) return false;
        return std::find(parents_.begin(), parents_.end(), parent) != parents_.end();
    }

    // --- Child Management ---
    bool addChild(GeometricObject* child) {
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

    bool removeChild(GeometricObject* child) {
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

    const std::vector<GeometricObject*>& getChildren() const {
        return children_;
    }

    bool hasChild(GeometricObject* child) const {
        if (!child) return false;
        return std::find(children_.begin(), children_.end(), child) != children_.end();
    }


protected:
    bool selected_;
    bool hovered_;
    bool legal_;
    bool hidden_;
    QString label_;
    std::vector<GeometricObject*> parents_ = {};
    std::vector<GeometricObject*> children_ = {};
};

#endif // GEOMETRICOBJECT_H
