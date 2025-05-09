#include "point.h"

Point::Point(const QPointF& position) : GeometricObject(ObjectName::Point), position_(position) {}

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
        painter->drawEllipse(position_, size_ + 1,  size_ + 1);
        if (selected_){
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(Qt::darkRed, 2));
            painter->drawEllipse(position_, size_ + 3, size_ + 3);
        }
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);    // Smooth edges
    painter->setBrush(color_); // Fill color
    painter->setPen(Qt::black); // Border color
    painter->drawEllipse(position_, size_, size_);

    if (selected_) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::darkRed, 2));
        painter->drawEllipse(position_, size_ + 2, size_ + 2);
    }

    painter->restore();
}

bool Point::isNear(const QPointF& clickPos) const {
    qreal dx = clickPos.x() - position().x();
    qreal dy = clickPos.y() - position().y();
    return (dx * dx + dy * dy) <= (size_ + 2) * (size_ + 2);
}
