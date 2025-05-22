#include "point.h"
#include "objecttype.h"
inline const QPointF footOfPerpendicular(QPointF P, std::pair<QPointF, QPointF> line, ObjectType mode=ObjectType::Line) {
    const QPointF& A = line.first;
    const QPointF& B = line.second;
    // 计算线段AB的向量
    QPointF AB = B - A;
    // 计算点P到点A的向量
    QPointF AP = P - A;
    // 计算AB的长度平方
    qreal abLengthSquared = AB.x() * AB.x() + AB.y() * AB.y();
    // 如果线段长度为0，返回端点A
    if (abLengthSquared == 0) return A;
    // 计算点积 AP·AB
    qreal dotProduct = AP.x() * AB.x() + AP.y() * AB.y();
    // 计算投影比例 t
    qreal t;
    if(mode==ObjectType::Lineoo){
       t = qBound(0.0, dotProduct / abLengthSquared, 1.0);
    }
    else if(mode == ObjectType::Lineo){
        t= qMax(0.0, dotProduct / abLengthSquared);
    }
    else{
        t= dotProduct / abLengthSquared;
    }
    // 计算垂足H
    return A + t * AB;
}

Point::Point(const QPointF& position) : GeometricObject(ObjectName::Point), position_(position) {
    GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
}

void Point::setPosition(const QPointF& pos) {
    switch(generation_){
    case 0:{
        expectParentNum(0);
        position_ = pos;
        return;
    }
    case 1:
    case 2:
    case 3:{
        expectParentNum(1);
        position_=footOfPerpendicular(pos,parents_[0]->getTwoPoints(),parents_[0]->getObjectType());
        return;
    }
    case 4:{

    }
    };
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
        painter->drawEllipse(position(), size_ + 1,  size_ + 1);
        if (selected_){
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(Qt::darkRed, 2));
            painter->drawEllipse(position(), size_ + 3, size_ + 3);
        }
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);    // Smooth edges
    painter->setBrush(color_); // Fill color
    painter->setPen(Qt::black); // Border color
    painter->drawEllipse(position(), size_, size_);

    if (selected_) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::darkRed, 2));
        painter->drawEllipse(position(), size_ + 2, size_ + 2);
    }

    painter->restore();
}

bool Point::isNear(const QPointF& clickPos) const {
    qreal dx = clickPos.x() - position().x();
    qreal dy = clickPos.y() - position().y();
    return (dx * dx + dy * dy) <= (size_ + 2) * (size_ + 2);
}
