#include "canvas.h"

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
}

void Canvas::setMode(Mode newMode) {
    currentMode = newMode;
}

void Canvas::setOperation(Operation* operation) {
    this->currentOperation = operation;
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

void Canvas::contextMenuEvent(QContextMenuEvent* event) {
    QPointF pos = event->pos();
    GeometricObject* contextMenuObj = findObjNear(pos);

    if (contextMenuObj && contextMenuObj->name == ObjectName::Point) {
        QMenu menu(this);
        Point* point = dynamic_cast<Point*>(contextMenuObj);

        QMenu* colorMenu = menu.addMenu("Color");
        colorMenu->addAction("Red", [this, point]() { point->setColor(Qt::red); update(); });
        colorMenu->addAction("Blue", [this, point]() { point->setColor(Qt::blue); update(); });
        colorMenu->addAction("Green", [this, point]() { point->setColor(Qt::green); update(); });
        colorMenu->addAction("Black", [this, point]() { point->setColor(Qt::black); update(); });
        /*colorMenu->addAction("Custom...", [this]() {
            QColor color = QColorDialog::getColor(contextMenuPoint->color(), this);
            if (color.isValid()) {
                contextMenuObj->setColor(color);
                update();
            }
        });*/

        QMenu* sizeMenu = menu.addMenu("Change Size");
        sizeMenu->addAction("Tiny", [this, point]() { point->setSize(2); update(); });
        sizeMenu->addAction("Small", [this, point]() { point->setSize(3); update(); });
        sizeMenu->addAction("Medium", [this, point]() { point->setSize(4); update(); });
        sizeMenu->addAction("Large", [this, point]() { point->setSize(5); update(); });

        menu.addAction("hide", [this, point]() { point->setHidden(true); update(); });

        menu.exec(event->globalPos());
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
