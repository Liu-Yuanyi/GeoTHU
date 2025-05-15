#include "operation.h"

// assertion: match exists
// use isValidInput beforehand
int Operation::getInputIndex(std::vector<GeometricObject*> objs) const {
    std::vector<ObjectType> types;
    for (auto obj : objs){
        types.push_back(obj->getObjectType());
    }
    for (int i = 0; i < inputType.size(); ++i){
        if (types == inputType[i]){
            return i;
        }
    }
    return -1; // This line should never be executed.
}

int Operation::isValidInput(std::vector<GeometricObject*> objs) const {
    std::vector<ObjectType> types;
    for (auto obj : objs){
        types.push_back(obj->getObjectType());
    }
    for (int i = 0; i < inputType.size(); ++i){
        if (types == inputType[i]){
            return 1;
        }
        if (types.size() > inputType[i].size()){
            continue;
        }
        std::vector<ObjectType> partialInput;
        for (int j = 0; j < types.size(); ++j){
            partialInput.push_back(inputType[i][j]);
        }
        if (types == partialInput){
            return 0;
        }
    }
    return -1;
}
