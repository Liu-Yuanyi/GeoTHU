#include "saveloadhelper.h"
#include "objecttype.h"
#include "point.h"
#include "line.h"
#include "lineo.h"
#include "lineoo.h"
#include "circle.h"

Saveloadhelper::Saveloadhelper() {}

void Saveloadhelper::save(GeometricObject* object, QDataStream& out) {
    out << object->legal_ << object->hidden_ << object->labelhidden_
        << object->label_ << object->color_ << object->size_ << object->shape_
        << object->generation_ << object->name_ << object->index_;
    if (object->name_ == ObjectName::Point) {
        out << object->position();
    }
    QVector<int> indices = {};
    for (auto parent : object->parents_) {
        indices.push_back(parent->index_);
    }
    out << indices;
}

GeometricObject* Saveloadhelper::load(QDataStream& in) {
    ObjectName name;
    bool legal, hidden, labelhidden;
    QString label;
    QColor color;
    double size;
    int shape, generation, index;
    QPointF position;
    GeometricObject* object = nullptr;
    in >> legal >> hidden >> labelhidden >> label >> color >> size
        >> shape >> generation >> name >> index;
    //qDebug() << GetObjectNameString(name);
    switch (name) {
    case (ObjectType::Point):
        in >> position;
        object = new Point(position);
        break;
    case (ObjectType::Line):
        object = new Line({}, 0);
        break;
    case (ObjectType::Lineo):
        object = new Lineo({}, 0);
        break;
    case (ObjectType::Lineoo):
        object = new Lineoo({}, 0);
        break;
    case (ObjectType::Circle):
        object = new Circle({}, 0);
        break;
    default:
        break;
    }
    allObjects.push_back(object);
    object->selected_ = false;
    object->hovered_ = false;
    object->legal_ = legal;
    object->hidden_ = hidden;
    object->labelhidden_ = labelhidden;
    object->label_ = label;
    object->color_ = color;
    object->size_ = size;
    object->shape_ = shape;
    object->generation_ = generation;
    object->name_ = name;
    object->index_ = index;
    QVector<int> indices = {};
    in >> indices;
    for (int index : indices) {
        for (auto obj : allObjects) {
            if (obj->index_ == index) {
                object->addParent(obj);
            }
        }
    }
    GeometricObject::counter = std::max(GeometricObject::counter, index + 1);
    return object;
}
