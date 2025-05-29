#include "intersectioncreator.h"
#include"point.h"

IntersectionCreator::IntersectionCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line    ,ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line    ,ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Line    ,ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo   ,ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo   ,ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo   ,ObjectType::Lineoo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo  ,ObjectType::Line});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo  ,ObjectType::Lineo});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo  ,ObjectType::Lineoo});

    inputType.push_back(std::vector<ObjectType>{ObjectType::Line    ,ObjectType::Circle});//9
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineo   ,ObjectType::Circle});//10
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo  ,ObjectType::Circle});//11
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle  ,ObjectType::Circle});//12
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle  ,ObjectType::Lineoo});//13
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle  ,ObjectType::Lineo});//14
    inputType.push_back(std::vector<ObjectType>{ObjectType::Circle  ,ObjectType::Line});//15
    operationName="IntersectionCreator";
}

std::set<GeometricObject*> IntersectionCreator::apply(std::vector<GeometricObject*> objs,
                                                       QPointF position)const{
    int index=getInputIndex(objs);
    std::set<GeometricObject*> ret;
    if(index<=8){
        GeometricObject *p=new Point(objs, index+5);
        p->flush();
        ret.insert(p);
    }
    else{
        if(index>=13){
            index=24-index;
            std::swap(objs[0],objs[1]);
        }
        GeometricObject *p1=new Point(objs, index*2-4);
        GeometricObject *p2=new Point(objs, index*2-3);
        p1->flush();
        p2->flush();
        ret.insert(p1);ret.insert(p2);
    }
    return ret;
}
