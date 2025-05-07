#include "point.h"

QColor defaultColor = Qt::red;
double defaultRadius = 4;

Point::Point(const QPointF& position) : GeometricObject(), position_(position), radius_(defaultRadius),
    color_(defaultColor) {
    name = ObjectName::Point;
}

void Point::setPosition(const QPointF& pos) {
    position_ = pos;
}

void Point::draw(QPainter* painter) const {
    if (hidden_) {
        return;
    }

    if (label_ != "") {
        painter->setPen(Qt::black);
        painter->drawText(position().x() + 6, position().y() - 6, label_);
    }

    if (hovered_) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::black);
        painter->drawEllipse(position_, radius_ + 1, radius_ + 1);
        if (selected_){
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(Qt::darkRed, 2));
            painter->drawEllipse(position_, radius_ + 3, radius_ + 3);
        }
        return;
    }

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

    painter->restore();
}

void Point::setColor(QColor color) {
    defaultColor = color;
    color_ = color;
}

QColor Point::getColor() {
    return color_;
}

void Point::setSize(double size) {
    defaultRadius = size;
    radius_ = size;
}

bool Point::isNear(const QPointF& clickPos) const {
    qreal dx = clickPos.x() - position().x();
    qreal dy = clickPos.y() - position().y();
    return (dx * dx + dy * dy) <= (radius_ + 2) * (radius_ + 2);
}
