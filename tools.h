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

class MidpointCreator: public Operation {
public:
    MidpointCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class PerpendicularLineCreator: public Operation {
public:
    PerpendicularLineCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class AngleBisectorCreator: public Operation {
public:
    AngleBisectorCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class TangentLineCreator: public Operation {
public:
    TangentLineCreator();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class AxialSymmetry: public Operation {
public:
    AxialSymmetry();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

class CentralSymmetry: public Operation {
public:
    CentralSymmetry();
    std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                      QPointF position = QPointF()) const override;
};

#endif // TOOLS_H
