#include "Line.h" // Updated to match the filename
#include <QPainter>
#include <QPen>
#include <cmath>    // For sqrt, fabs, pow
#include <algorithm> // For std::min, std::max

// Default values, similar to Point class for consistency if desired
QColor defaultLineColor = Qt::black;
double defaultLineThickness = 1.0;

Line::Line(const QPointF& p1, const QPointF& p2, QColor color, double thickness)
    : GeometricObject(), startPoint_(p1), endPoint_(p2), color_(color), thickness_(thickness) {
    name = ObjectName::Line; // Set the object name, assuming ObjectName::Line is defined in objecttype.h
    if (color_ == QColor()) { // Handle default constructor QColor case
        color_ = defaultLineColor;
    }
}

void Line::draw(QPainter* painter) const {
    if (isHidden()) { // Use getter from base class
        return;
    }

    painter->save();

    QPen pen(color_, thickness_);
    pen.setCapStyle(Qt::RoundCap); // Makes line ends look a bit nicer

    if (isHovered()) {
        QPen hoverPen(color_, thickness_ + 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // Thicker line for hover
        if (isSelected()) {
            hoverPen.setColor(Qt::darkRed); // Different color if also selected
        } else {
            hoverPen.setColor(Qt::red); // Hover color
        }
        painter->setPen(hoverPen);
    } else if (isSelected()) {
        QPen selectedPen(Qt::darkRed, thickness_ + 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin); // Dashed line for selection
        painter->setPen(selectedPen);
    } else {
        painter->setPen(pen);
    }

    painter->drawLine(startPoint_, endPoint_);

    // Optionally draw label (if label_ is not empty)
    if (!getLabel().isEmpty()) { // Use getter from base class
        painter->setPen(Qt::black); // Reset pen for text or use a specific text color
        // Position the label near the midpoint or start point
        QPointF labelPos = (startPoint_ + endPoint_) / 2.0 + QPointF(5, -5);
        painter->drawText(labelPos, getLabel());
    }

    painter->restore();
}

void Line::setColor(QColor color) {
    color_ = color;
}

void Line::setSize(double size) {
    thickness_ = std::max(0.5, size); // Ensure thickness is not too small
}

// Helper function for distance from point p to line segment p1p2
double Line::distanceToSegment(const QPointF& p, const QPointF& segP1, const QPointF& segP2) const {
    double l2 = (segP2.x() - segP1.x()) * (segP2.x() - segP1.x()) + (segP2.y() - segP1.y()) * (segP2.y() - segP1.y());
    if (l2 == 0.0) return std::sqrt((p.x() - segP1.x()) * (p.x() - segP1.x()) + (p.y() - segP1.y()) * (p.y() - segP1.y())); // segP1 == segP2

    // Consider the line extending the segment is P = segP1 + t (segP2 - segP1).
    // t = [(p-segP1) . (segP2-segP1)] / |segP2-segP1|^2
    double t = ((p.x() - segP1.x()) * (segP2.x() - segP1.x()) + (p.y() - segP1.y()) * (segP2.y() - segP1.y())) / l2;
    t = std::max(0.0, std::min(1.0, t)); // Clamp t to [0,1] to stay on the segment

    QPointF projection = segP1 + t * (segP2 - segP1); // Projected point on segment
    return std::sqrt(std::pow(p.x() - projection.x(), 2) + std::pow(p.y() - projection.y(), 2));
}


bool Line::isNear(const QPointF& pos) const {
    if (isHidden()) return false;
    return distanceToSegment(pos, startPoint_, endPoint_) < (LINE_NEAR_TOLERANCE + thickness_ / 2.0);
}

void Line::setPosition(const QPointF& pos) {
    QPointF offset = pos - startPoint_;
    startPoint_ = pos;
    endPoint_ += offset; // Move endPoint relative to startPoint
}

QPointF Line::position() const {
    return startPoint_; // Define line's "position" as its start point
}

void Line::setStartPoint(const QPointF& p) {
    startPoint_ = p;
}

void Line::setEndPoint(const QPointF& p) {
    endPoint_ = p;
}

void Line::setPoints(const QPointF& p1, const QPointF& p2) {
    startPoint_ = p1;
    endPoint_ = p2;
}
