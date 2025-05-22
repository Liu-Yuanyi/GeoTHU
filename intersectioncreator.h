#ifndef INTERSECTIONCREATOR_H
#define INTERSECTIONCREATOR_H

#include "geometricobject.h"
#include"operation.h"
#include"objecttype.h"

class IntersectionCreator: public Operation{
    IntersectionCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

#endif // INTERSECTIONCREATOR_H
