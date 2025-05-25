#ifndef INTERSECTIONCALCULATOR_H
#define INTERSECTIONCALCULATOR_H

#include"point.h"

struct linelineIntersectionResult{
    QPointF p;
    double t1,t2;
    bool legal;
};

struct linecircleIntersectionResult{
    QPointF p1,p2;
    double t1,t2;
    bool legal;
};

struct circlecircleIntersectionResult{
    QPointF p1,p2;
    double t1,t2;
    bool legal;
};

#endif // INTERSECTIONCALCULATOR_H
