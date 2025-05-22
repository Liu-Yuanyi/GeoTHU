#ifndef TOOLS_H
#define TOOLS_H
#include"operation.h"

class PerpendicularBisectorCreator: public Operation{
public:
    PerpendicularBisectorCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class ParallelLineCreator: public Operation{
public:
    ParallelLineCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class IntersectionCreator: public Operation{
    IntersectionCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

#endif // TOOLS_H
