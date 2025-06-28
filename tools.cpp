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
    pLine->flush();
    std::set<GeometricObject*> ret{pLine};
    return ret;
}

ParallelLineCreator::ParallelLineCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point,ObjectType::Point});
    operationName="ParallelLineCreator";
}

std::set<GeometricObject*> ParallelLineCreator::apply(std::vector<GeometricObject*> objs,
                                                       QPointF position)const{
    if(objs[0]->getObjectType()==ObjectType::Point && objs.size()==2){
        std::swap(objs[1],objs[0]);
    }
    Line *pLine=new Line(objs,3);
    pLine->flush();
    std::set<GeometricObject*> ret{pLine};
    return ret;
}

MidpointCreator::MidpointCreator() {
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Point});
    operationName = "MidPointCreator";
}

std::set<GeometricObject*> MidpointCreator::apply(std::vector<GeometricObject*> objs,
                                                   QPointF position) const {
    Point* newPoint = new Point(objs, 30);
    newPoint->flush();
    std::set<GeometricObject*> ret{newPoint};
    return ret;
}

PerpendicularLineCreator::PerpendicularLineCreator() {
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line,  ObjectType::Point});
    operationName = "PerpendicularLineCreator";
}

std::set<GeometricObject*> PerpendicularLineCreator::apply(std::vector<GeometricObject*> objs,
                                                            QPointF position) const {
    if(objs[1]->getObjectType()==ObjectType::Point){
        std::swap(objs[0],objs[1]);
    }
    Line* newLine = new Line(objs, 6);
    newLine->flush();
    std::set<GeometricObject*> ret{newLine};
    return ret;
}

AngleBisectorCreator::AngleBisectorCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Point, ObjectType::Point});
    operationName = "AngleBisectorCreator";
}

std::set<GeometricObject*> AngleBisectorCreator::apply(std::vector<GeometricObject*> objs,
                                                        QPointF position) const {
    Lineo* newLineo = new Lineo(objs, 1);
    newLineo->flush();
    std::set<GeometricObject*> ret{newLineo};
    return ret;
}

TangentLineCreator::TangentLineCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Circle});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Point});

    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Arc});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Arc, ObjectType::Point});

    operationName = "TangentLineCreator";
}

std::set<GeometricObject*> TangentLineCreator::apply(std::vector<GeometricObject*> objs,
                                                      QPointF position) const {
    if(objs[1]->getObjectType()==ObjectType::Point){
        std::swap(objs[0],objs[1]);
    }
    if(objs[1]->getObjectType()==ObjectType::Circle){
        Circle* circle = dynamic_cast<Circle*>(objs[1]);
        QPointF p1 = objs[0]->position(), p2 = circle->position();
        long double radius = circle->getRadius();
        long double dist = std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
        if (radius > dist + 1e-6) {
            return std::set<GeometricObject*>();
        } else if (abs(radius - dist) <= 1e-6) {
            return std::set<GeometricObject*>{(new Line(objs, 7))->flush()};
        } else {
            return std::set<GeometricObject*>{(new Line(objs, 8))->flush(), (new Line(objs, 9))->flush(), (new Point(objs,31))->flush(), (new Point(objs,32))->flush()};
        }
    }
    else{
        Arc* arc = dynamic_cast<Arc*>(objs[1]);
        QPointF p1 = objs[0]->position(), p2 = arc->position();
        long double radius = arc->getRadius();
        long double dist = std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
        if (radius > dist + 1e-6) {
            return std::set<GeometricObject*>();
        } else if (abs(radius - dist) <= 1e-6) {
            return std::set<GeometricObject*>{(new Line(objs, 7))->flush()};
        } else {
            return std::set<GeometricObject*>{(new Line(objs, 8))->flush(), (new Line(objs, 9))->flush(), (new Point(objs,31))->flush(), (new Point(objs,32))->flush()};
        }

    }
}

AxialSymmetry::AxialSymmetry(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Arc, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Arc, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Arc, ObjectType::Lineoo});

    operationName = "AxialSymmetry";
}

std::set<GeometricObject*> AxialSymmetry::apply(std::vector<GeometricObject*> objs,
                                                 QPointF position) const {
    GeometricObject *ret;
    switch(objs[0]->getObjectType()){
    case ObjectType::Circle:{
        return std::set<GeometricObject*>{(new Circle(objs, -3))->flush()};
        break;
    }
    case ObjectType::Arc:{
        return std::set<GeometricObject*>{(new Arc(objs, -3))->flush()};
        break;
    }
    case ObjectType::Line:{
        return std::set<GeometricObject*>{(new Line(objs, -3))->flush()};
        break;
    }
    case ObjectType::Lineo:{
        return std::set<GeometricObject*>{(new Lineo(objs, -3))->flush()};
        break;
    }
    case ObjectType::Lineoo:{
        return std::set<GeometricObject*>{(new Lineoo(objs, -3))->flush()};
        break;
    }
    case ObjectType::Point:{
        return std::set<GeometricObject*>{(new Point(objs, -3))->flush()};
        break;
    }
    default:{
        QMessageBox::warning(nullptr, "警告", "尝试对Any/None对象进行几何变换!");
        return std::set<GeometricObject*>();
    }
    }
}

CentralSymmetry::CentralSymmetry(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Arc, ObjectType::Point});

    operationName = "CentralSymmetry";
}

std::set<GeometricObject*> CentralSymmetry::apply(std::vector<GeometricObject*> objs,
                                                   QPointF position) const {
    GeometricObject *ret;
    switch(objs[0]->getObjectType()){
    case ObjectType::Circle:{
        return std::set<GeometricObject*>{(new Circle(objs, -4))->flush()};
        break;
    }
    case ObjectType::Arc:{
        return std::set<GeometricObject*>{(new Arc(objs, -4))->flush()};
        break;
    }
    case ObjectType::Line:{
        return std::set<GeometricObject*>{(new Line(objs, -4))->flush()};
        break;
    }
    case ObjectType::Lineo:{
        return std::set<GeometricObject*>{(new Lineo(objs, -4))->flush()};
        break;
    }
    case ObjectType::Lineoo:{
        return std::set<GeometricObject*>{(new Lineoo(objs, -4))->flush()};
        break;
    }
    case ObjectType::Point:{
        return std::set<GeometricObject*>{(new Point(objs, -4))->flush()};
        break;
    }
    default:{
        QMessageBox::warning(nullptr, "警告", "尝试对Any/None对象进行几何变换!");
        return std::set<GeometricObject*>();
    }
    }
}
