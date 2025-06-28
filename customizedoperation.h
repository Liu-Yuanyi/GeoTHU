#ifndef CUSTOMIZEDOPERATION_H
#define CUSTOMIZEDOPERATION_H

#include "operation.h"
#include "geometricobject.h"

class CustomizedOperation : public Operation {
private:
    friend class CustomizedOperationCreator;
    mutable std::vector<GeometricObject*> objsRelated;

    // [ ([parents' indices], generation, objecttype, aux) ]
    std::vector<std::tuple<std::vector<int>, int, ObjectType, bool>> applyOrder;

public:
    CustomizedOperation(QString name);
    virtual std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                             QPointF position = QPointF()) const override;
};

class CustomizedOperationCreator {
private:
    std::set<GeometricObject*> getInput(std::set<GeometricObject*> selectedObjs);

public:
    bool canApply(std::set<GeometricObject*> selectedObjs);
    CustomizedOperation* apply(std::set<GeometricObject*> selectedObjs, QString name);
};

#endif // CUSTOMIZEDOPERATION_H
