#ifndef OPERATION_H
#define OPERATION_H

#include "geometricobject.h"
#include <QWidget>

class Operation {
protected:
    std::vector<std::vector<ObjectType>> inputType;
    std::string operationName;
    virtual int getInputIndex(std::vector<GeometricObject*> objs) const = 0;

public:
    virtual int isValidInput(std::vector<GeometricObject*> objs) const = 0;
    std::string getName() {return operationName;}
    virtual std::set<GeometricObject*> apply(std::vector<GeometricObject*> objs,
                                              QPointF position = QPointF()) const = 0;
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
