#include "intersectioncreator.h"

IntersectionCreator::IntersectionCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo,ObjectType::Lineoo});
    operationName="IntersectionCreator";
}

std::set<GeometricObject*> IntersectionCreator::apply(std::vector<GeometricObject*> objs,
                                                       QPointF position)const{

}
