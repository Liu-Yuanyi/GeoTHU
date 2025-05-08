#ifndef LINE_H
#define LINE_H

#include "geometricobject.h" // Updated to match the provided filename
#include <QPointF>
#include <QColor>
#include <QPen>   // For QPen in draw method
#include <cmath>  // For std::sqrt, std::fabs in isNear

// A small tolerance for isNear checks (e.g., in pixels)
const double LINE_NEAR_TOLERANCE = 5.0;

class Line : public GeometricObject {
public:
    // Constructor
    explicit Line(const QPointF& p1, const QPointF& p2, QColor color = Qt::black, double thickness = 1.0);

    // Override pure virtual functions from GeometricObject
    void draw(QPainter* painter) const override;
    void setColor(QColor color) override;
    void setSize(double size) override; // Interpreted as thickness
    bool isNear(const QPointF& pos) const override;
    void setPosition(const QPointF& pos) override; // Moves startPoint_ to pos, endPoint_ relative
    QPointF position() const override; // Returns startPoint_

    // Line specific getters and setters
    QPointF getStartPoint() const { return startPoint_; }
    void setStartPoint(const QPointF& p);
    QPointF getEndPoint() const { return endPoint_; }
    void setEndPoint(const QPointF& p);
    void setPoints(const QPointF& p1, const QPointF& p2);

    QColor getColor() const { return color_; }
    double getThickness() const { return thickness_; }


private:
    QPointF startPoint_;
    QPointF endPoint_;
    QColor color_;
    double thickness_;

    // Helper for isNear calculation (distance from point to line segment)
    double distanceToSegment(const QPointF& p, const QPointF& segP1, const QPointF& segP2) const;
};

#endif // LINE_H
