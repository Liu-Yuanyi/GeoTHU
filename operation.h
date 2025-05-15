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
    int isValidInput(std::vector<GeometricObject*> objs) const; // -1 不可能符合
                                                                // 0  可能符合，但未输入完成
                                                                // 1  完全符合
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
