#include "tools.h"
#include"point.h"
#include"line.h"
#include"lineo.h"
#include"lineoo.h"
#include"circle.h"

PerpendicularBisectorCreator::PerpendicularBisectorCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo});
    operationName="PerpendicularBisectorCreator";
}

std::set<GeometricObject*> PerpendicularBisectorCreator::apply(std::vector<GeometricObject*> objs,
                                         QPointF position)const{
    int generation=PerpendicularBisectorCreator::getInputIndex(objs);
    Line *pLine=new Line(objs,1+generation);
    std::set<GeometricObject*> ret{pLine};
    return ret;
}

ParallelLineCreator::ParallelLineCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point,ObjectType::Point});
    operationName="ParallelLineCreator";
}

std::set<GeometricObject*> ParallelLineCreator::apply(std::vector<GeometricObject*> objs,
                                                                QPointF position)const{
    Line *pLine=new Line(objs,3);
    std::set<GeometricObject*> ret{pLine};
    return ret;
}
