#include "customizedoperation.h"
#include "point.h"
#include "line.h"
#include "lineo.h"
#include "lineoo.h"
#include "circle.h"
#include "measurement.h"

std::set<GeometricObject*> ancestor(GeometricObject* obj){
    if (obj->getParents().empty()){
        return {};
    }
    std::set<GeometricObject*> ret = {};
    for (auto parent : obj->getParents()){
        ret.insert(parent);
        for (auto grandparent : ancestor(parent)){
            ret.insert(grandparent);
        }
    }
    return ret;
}

bool isDeterminedBy(std::set<GeometricObject*> objs, GeometricObject* obj){
    if (objs.empty()){
        return true;
    }
    if (objs.find(obj) != objs.end()){
        return true;
    }
    if (obj->getParents().empty()){
        return false;
    }
    for (auto parent : obj->getParents()) {
        if (!isDeterminedBy(objs, parent)){
            return false;
        }
    }
    return true;
}

std::set<GeometricObject*> CustomizedOperationCreator::getInput(std::set<GeometricObject*> selectedObjs){
    std::set<GeometricObject*> input;
    for (auto obj : selectedObjs){
        bool isTop = true;
        for (auto anc : ancestor(obj)){
            if (selectedObjs.find(anc) != selectedObjs.end()){
                isTop = false;
                break;
            }
        }
        if (isTop) {
            input.insert(obj);
        }
    }
    return input;
}

bool CustomizedOperationCreator::canApply(std::set<GeometricObject*> selectedObjs){
    if (selectedObjs.empty()){
        return false;
    }
    std::set<GeometricObject*> input = getInput(selectedObjs);
    if (input.empty() or input.size() == selectedObjs.size()) {
        return false;
    }
    for (auto obj : selectedObjs){
        if (!isDeterminedBy(input, obj)){
            return false;
        }
        if (obj->getObjectType() == ObjectType::Measurement){
            for (auto parent : obj->getParents()) {
                if (selectedObjs.find(parent) == selectedObjs.end()){
                    return false;
                }
            }
        }
    }
    return true;
}

std::vector<std::vector<ObjectType>> permutations(std::vector<ObjectType> v){
    std::vector<std::vector<ObjectType>> result;
    std::sort(v.begin(), v.end());
    do {
        result.push_back(v);
    } while (std::next_permutation(v.begin(), v.end()));
    return result;
}

void traceBack(std::vector<GeometricObject*>& v, std::set<GeometricObject*> input, GeometricObject* target){
    if (input.find(target) != input.end()){
        return;
    } else if (std::find(v.begin(), v.end(), target) != v.end()){
        return;
    }
    v.push_back(target);
    for (auto parent : target->getParents()){
        traceBack(v, input, parent);
    }
}

template <typename T>
int findIndex(const T& element, const std::vector<T>& vec) {
    for (std::size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == element) {
            return static_cast<int>(i);  // Cast to match return type
        }
    }
    return -1;
}

CustomizedOperation* CustomizedOperationCreator::apply(std::set<GeometricObject*> selectedObjs, QString name){
    CustomizedOperation* ret = new CustomizedOperation(name);
    std::set<GeometricObject*> input = getInput(selectedObjs);
    std::set<GeometricObject*> output = {};
    std::vector<ObjectType> inputType = {};

    for (auto obj : selectedObjs) {
        if (input.find(obj) == input.end()){
            output.insert(obj);
        } else {
            inputType.push_back(obj->getObjectType());
        }
    }
    ret->inputType = permutations(inputType);

    std::vector<GeometricObject*> relatedObjs = {};
    for (auto obj : input) {
        relatedObjs.push_back(obj);
    }
    std::stable_sort(relatedObjs.begin(), relatedObjs.end(),
                     [](GeometricObject* a, GeometricObject* b) { return *a < *b; });
    std::vector<GeometricObject*> objsToConstruct = {};
    for (auto obj : output) {
        traceBack(objsToConstruct, input, obj);
    }
    std::sort(objsToConstruct.begin(), objsToConstruct.end(),
              [](GeometricObject* a, GeometricObject* b) { return a->getIndex() < b->getIndex(); });

    // [ ([parents' indices], generation, objecttype, aux) ]
    std::vector<std::tuple<std::vector<int>, int, ObjectType, bool>> applyOrder = {};
    for (auto obj : objsToConstruct) {
        std::vector<GeometricObject*> parents = obj->getParents();
        std::vector<int> indices = {};
        for (auto parent : parents){
            int index = findIndex(parent, relatedObjs);
            if (index == -1){
                qDebug() << "error!";
            }
            indices.push_back(index);
        }
        bool aux = (output.find(obj) == output.end());
        applyOrder.push_back({indices, obj->getGeneration(), obj->getObjectType(), aux});
        relatedObjs.push_back(obj);
    }
    ret->applyOrder = applyOrder;

    return ret;
}

CustomizedOperation::CustomizedOperation(QString name){
    operationName = name.toStdString();
}

std::set<GeometricObject*> CustomizedOperation::apply(std::vector<GeometricObject*> objs,
                                                      QPointF position) const {
    objsRelated.clear();
    objsRelated = objs;
    std::stable_sort(objsRelated.begin(), objsRelated.end(),
                     [](GeometricObject* a, GeometricObject* b) { return *a < *b; });
    std::set<GeometricObject*> ret = {};
    for (auto& t : applyOrder) {
        std::vector<int> indices;
        std::vector<GeometricObject*> parents;
        int generation;
        ObjectType type;
        bool aux;
        GeometricObject* newObj = nullptr;
        std::tie(indices, generation, type, aux) = t;
        for (int i : indices) {
            parents.push_back(objsRelated[i]);
        }
        switch (type) {
        case (ObjectType::Point):
            newObj = new Point(parents, generation, aux);
            break;
        case (ObjectType::Line):
            newObj = new Line(parents, generation, false, aux);
            break;
        case (ObjectType::Lineo):
            newObj = new Lineo(parents, generation, false, aux);
            break;
        case (ObjectType::Lineoo):
            newObj = new Lineoo(parents, generation, false, aux);
            break;
        case (ObjectType::Circle):
            newObj = new Circle(parents, generation, false, aux);
            break;
        case (ObjectType::Arc):
            newObj = new Arc(parents, generation, false, aux);
            break;
        case (ObjectType::Measurement):
            newObj = new Measurement(parents, generation, false, aux);
            break;
        default:
            break;
        }
        newObj->flush();
        objsRelated.push_back(newObj);
        ret.insert(newObj);
    }
    objsRelated.clear();
    return ret;
}
