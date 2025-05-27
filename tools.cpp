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

MidpointCreator::MidpointCreator() {
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Point});
    operationName = "MidPointCreator";
}

std::set<GeometricObject*> MidpointCreator::apply(std::vector<GeometricObject*> objs,
                                                       QPointF position) const {
    Point* newPoint = new Point(objs, 30);
    std::set<GeometricObject*> ret{newPoint};
    return ret;
}

PerpendicularLineCreator::PerpendicularLineCreator() {
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Line});
    operationName = "PerpendicularLineCreator";
}

std::set<GeometricObject*> PerpendicularLineCreator::apply(std::vector<GeometricObject*> objs,
                                                   QPointF position) const {
    Line* newLine = new Line(objs, 6);
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
    std::set<GeometricObject*> ret{newLineo};
    return ret;
}

TangentLineCreator::TangentLineCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Circle});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Point});

    operationName = "TangentLineCreator";
}

std::set<GeometricObject*> TangentLineCreator::apply(std::vector<GeometricObject*> objs,
                                                        QPointF position) const {
    if(objs[0]->getObjectType()==ObjectType::Circle){
        std::swap(objs[0],objs[1]);
    }
    Circle* circle = dynamic_cast<Circle*>(objs[1]);
    QPointF p1 = objs[0]->position(), p2 = circle->position();
    double radius = circle->getRadius();
    double dist = std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
    if (radius > dist + 1e-4) {
        return std::set<GeometricObject*>();
    } else if (abs(radius - dist) <= 1e-4) {
        return std::set<GeometricObject*>{new Line(objs, 7)};
    } else {
        return std::set<GeometricObject*>{new Line(objs, 8), new Line(objs, 9), new Point(objs,31), new Point(objs,32)};
    }
}

AxialSymmetry::AxialSymmetry(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo, ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle, ObjectType::Lineoo});

    operationName = "AxialSymmetry";
}

std::set<GeometricObject*> AxialSymmetry::apply(std::vector<GeometricObject*> objs,
                                                      QPointF position) const {
    GeometricObject *ret;
    switch(objs[0]->getObjectType()){
    case ObjectType::Circle:{
        QMessageBox::warning(nullptr, "警告", "Circle暂时不兼容几何变换!");
        return std::set<GeometricObject*>();
        //return std::set<GeometricObject*>{new Circle(objs, 7)};
        break;
    }
    case ObjectType::Line:{
        return std::set<GeometricObject*>{new Line(objs, -3)};
        break;
    }
    case ObjectType::Lineo:{
        return std::set<GeometricObject*>{new Lineo(objs, -3)};
        break;
    }
    case ObjectType::Lineoo:{
        return std::set<GeometricObject*>{new Lineoo(objs, -3)};
        break;
    }
    case ObjectType::Point:{
        return std::set<GeometricObject*>{new Point(objs, -3)};
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

    operationName = "CentralSymmetry";
}

std::set<GeometricObject*> CentralSymmetry::apply(std::vector<GeometricObject*> objs,
                                                 QPointF position) const {
    GeometricObject *ret;
    switch(objs[0]->getObjectType()){
    case ObjectType::Circle:{
        QMessageBox::warning(nullptr, "警告", "Circle暂时不兼容几何变换!");
        return std::set<GeometricObject*>();
        //return std::set<GeometricObject*>{new Circle(objs, 7)};
        break;
    }
    case ObjectType::Line:{
        return std::set<GeometricObject*>{new Line(objs, -4)};
        break;
    }
    case ObjectType::Lineo:{
        return std::set<GeometricObject*>{new Lineo(objs, -4)};
        break;
    }
    case ObjectType::Lineoo:{
        return std::set<GeometricObject*>{new Lineoo(objs, -4)};
        break;
    }
    case ObjectType::Point:{
        return std::set<GeometricObject*>{new Point(objs, -4)};
        break;
    }
    default:{
        QMessageBox::warning(nullptr, "警告", "尝试对Any/None对象进行几何变换!");
        return std::set<GeometricObject*>();
    }
    }
}
