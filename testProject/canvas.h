#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>
#include "geometricobject.h"
#include "point.h"

class Canvas : public QWidget {
    Q_OBJECT
public:
    enum Mode { SelectionMode, CreatePointMode };
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas() override { qDeleteAll(objects_); }
    void setMode(Mode newMode);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<GeometricObject*> objects_;
    Mode currentMode = SelectionMode;
    GeometricObject* hoveredObj_ = nullptr;
    GeometricObject* draggedObj_ = nullptr;
    std::set<GeometricObject*> selectedObjs_ = {};
    QPointF mousePos_;
    std::map<GeometricObject*, QPointF> initialPositions_;
    bool deselectPermitted = true;

    void updateHoverState(const QPointF& pos);
    GeometricObject* findObjNear(const QPointF& pos) const;
    Point* findPointNear(const QPointF& pos) const;
    void clearSelections();

};

#endif // CANVAS_H
