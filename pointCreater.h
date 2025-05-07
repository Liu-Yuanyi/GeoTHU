#ifndef POINTCREATER_H
#define POINTCREATER_H

#include "operation.h"
#include "point.h"

class PointCreater: public Operation {
public:
    bool isValidInput() const override {
        return true;
    }

    GeometricObject* apply(QPointF position) const override {
        return new Point(position);
    }
};

#endif // POINTCREATER_H
