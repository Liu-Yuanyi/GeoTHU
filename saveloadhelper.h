#ifndef SAVELOADHELPER_H
#define SAVELOADHELPER_H

#include "geometricobject.h"

class Saveloadhelper
{
public:
    Saveloadhelper();
    void save(GeometricObject* object, QDataStream& out);
    GeometricObject* load(QDataStream& in);

private:
    std::vector<GeometricObject*> allObjects = {};
};

#endif // SAVELOADHELPER_H
