#include "point.h"

Qt::GlobalColor defaultColor = Qt::red;
double defaultRadius = 4;

Point::Point(const QPointF& position) : GeometricObject(), position_(position), radius_(defaultRadius),
    color_(defaultColor) {
    name = "point";
}

void Point::setPosition(const QPointF& pos) {
    position_ = pos;
}

void Point::draw(QPainter* painter) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);    // Smooth edges
    painter->setBrush(color_); // Fill color
    painter->setPen(Qt::black); // Border color
    painter->drawEllipse(position_, radius_, radius_);

    if (selected_) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::darkRed, 2));
        painter->drawEllipse(position_, radius_ + 2, radius_ + 2);
    }

    if (hovered_) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::black);
        painter->drawEllipse(position_, radius_ + 2, radius_ + 2);
    }

    painter->restore();
}

bool Point::isNear(const QPointF& clickPos) const {
    qreal dx = clickPos.x() - position().x();
    qreal dy = clickPos.y() - position().y();
    return (dx * dx + dy * dy) <= (radius_ + 2) * (radius_ + 2);
}
