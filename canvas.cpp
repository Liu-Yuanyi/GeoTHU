#include "canvas.h"

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
}

void Canvas::setMode(Mode newMode) {
    currentMode = newMode;
}

void Canvas::setOperation(Operation* operation) {
    this->operation = operation;
}

void Canvas::updateHoverState(const QPointF& pos) {
    GeometricObject* newHover = findObjNear(pos);
    if (newHover != hoveredObj_) {
        if (hoveredObj_) {
            hoveredObj_->setHovered(false);
        }
        hoveredObj_ = newHover;
        if (hoveredObj_) {
            hoveredObj_->setHovered(true);
        }
        update();
    }
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mousePos_ = event->position();
        initialPositions_.clear();

        if (currentMode == SelectionMode) {
            GeometricObject* clickedObj = findObjNear(mousePos_);

            if (clickedObj and !clickedObj->isSelected()) {
                clickedObj->setSelected(true);
                selectedObjs_.insert(clickedObj);
                deselectPermitted = false;
            }
            if (!clickedObj) {
                clearSelections();
            }
        }
        else if (currentMode == CreatePointMode) {
            // Only create if not near existing point
            if (!findPointNear(mousePos_)) {
                clearSelections();
                GeometricObject* newPoint = new Point(mousePos_);
                objects_.push_back(newPoint);
                selectedObjs_.insert(newPoint);
                deselectPermitted = false;
            } else {
                Point* point = findPointNear(mousePos_);
                clearSelections();
                point->setSelected(true);
                selectedObjs_.insert(point);
                deselectPermitted = false;
            }
        }

        for (auto obj : selectedObjs_) {
            initialPositions_[obj] = obj->position();
        }

        update();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentPos = event->position();

    if (currentMode == CreatePointMode) {
        updateHoverState(currentPos);
    }

    if (currentMode == SelectionMode){
        if (!selectedObjs_.empty() && (event->buttons() & Qt::LeftButton)) {
            // Update position while dragging
            QPointF delta = currentPos - mousePos_;
            // Update all selected objects relative to their initial positions
            for (auto obj : selectedObjs_) {
                QPointF newPos = initialPositions_[obj] + delta;
                obj->setPosition(newPos);
            }
            deselectPermitted = false;
            update();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !deselectPermitted) {
        deselectPermitted = true;
    }
    else if (event->button() == Qt::LeftButton && deselectPermitted) {
        mousePos_ = event->position();

        if (currentMode == SelectionMode) {
            GeometricObject* clickedObj = findObjNear(mousePos_);
            if (clickedObj and clickedObj->isSelected()) {
                clickedObj->setSelected(false);
                selectedObjs_.erase(clickedObj);
            }
        }

        update();
    }
}

void Canvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for (const auto* obj : objects_) {
        obj->draw(&painter);
    }
}

GeometricObject* Canvas::findObjNear(const QPointF& pos) const {
    for (GeometricObject* obj : objects_) {
        if (obj->isNear(pos)){
            return obj;
        }
    }
    return nullptr;
}

Point* Canvas::findPointNear(const QPointF& pos) const {
    for (auto* obj : objects_) {
        if (obj->name == ObjectName::Point and obj->isNear(pos)) {
            Point* point = dynamic_cast<Point*>(obj);
            return point;
        }
    }
    return nullptr;
}

void Canvas::clearSelections() {
    for (auto* obj : objects_) {
        obj->setSelected(false);
    }
    selectedObjs_.clear();
}
