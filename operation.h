#ifndef OPERATION_H
#define OPERATION_H

#include "geometricobject.h"
#include <QWidget>

class Operation {
protected:
    std::vector<std::vector<ObjectType>> inputType;
    std::string operationName;
    int getInputIndex(std::vector<GeometricObject*> objs) const;

public:
    int isValidInput(std::vector<GeometricObject*> objs) const;
    bool isWaiting(std::vector<GeometricObject*> objs) const;
    std::string getName() { return operationName; }
    virtual std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                              QPointF position = QPointF()) const = 0;
    virtual std::set<GeometricObject*> wait(std::vector<GeometricObject*> objs) const;
    virtual ~Operation();
};

#endif // OPERATION_H



/*currentOperation= operations[7];
for select:
             if(cO->isvalid(selected)==0){
        quxiao;
        break;
    }
if(==1){
    continue;
}
if(==2){
    add(apply(input));
    quxiao
    break
}*/
