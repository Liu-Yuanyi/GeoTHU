#ifndef OPERATION_H
#define OPERATION_H

#include "geometricobject.h"
#include <QWidget>

class Operation {
protected:
    std::set<std::set<GeometricObject*>> input;

public:
    virtual bool isValidInput() const = 0;
    virtual GeometricObject* apply(QPointF position = QPointF()) const = 0;
};

#endif // OPERATION_H
