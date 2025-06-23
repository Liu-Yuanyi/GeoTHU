#include "operation.h"

Operation::~Operation(){}

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

//0 找到完美符合
//1 确定不可能符合
//2 有可能符合，且下一个一定是点
//3 有可能符合，且下一个不可能是点
//4 有可能符合，且下一个有可能是点，但不一定是点
int Operation::isValidInput(std::vector<GeometricObject*> objs) const {
    std::vector<ObjectType> types;
    for (auto obj : objs){
        types.push_back(obj->getObjectType());
    }
    for (int i = 0; i < inputType.size(); ++i){
        if (types == inputType[i]){
            return 0;
        }
    }
    std::set<ObjectType> possibleNext = {};
    for (int i = 0; i < inputType.size(); ++i){
        if (types.size() > inputType[i].size()){
            continue;
        }
        std::vector<ObjectType> partialInput;
        for (int j = 0; j < types.size(); ++j){
            partialInput.push_back(inputType[i][j]);
        }
        if (types == partialInput){
            possibleNext.insert(inputType[i][types.size()]);
        }
    }
    if (possibleNext.empty()){
        return 1;
    } else if (possibleNext.find(ObjectType::Point) == possibleNext.end()){
        return 3;
    } else {
        if (possibleNext.size() == 1){
            return 2;
        }
        else {
            return 4;
        }
    }
}

std::set<GeometricObject*> Operation::wait(std::vector<GeometricObject*> objs) const {
    return {};
}

bool Operation::isWaiting(std::vector<GeometricObject*> objs) const {
    std::vector<ObjectType> types;
    for (auto obj : objs){
        types.push_back(obj->getObjectType());
    }
    for (auto v : inputType) {
        types.push_back(v[v.size() - 1]);
        if (types == v and v[v.size() - 1] == ObjectType::Point) {
            return true;
        }
    }
    return false;
}
