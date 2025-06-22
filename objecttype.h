#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

#include<string>

enum class ObjectName {
    None, // 默认或未定义类型
    Point,
    Line,
    Lineo,
    Lineoo,
    Circle,
    Arc,
    Any,
};

typedef ObjectName ObjectType;

inline std::string GetObjectNameString(ObjectName name){
    switch(name){
    case ObjectName::None: return "None";
    case ObjectName::Point: return "Point";
    case ObjectName::Line: return "Line";
    case ObjectName::Lineo: return "Lineo";
    case ObjectName::Lineoo: return "Lineoo";
    case ObjectName::Circle: return "Circle";
    case ObjectName::Arc: return "Arc";
    case ObjectName::Any: return "Any";
        // ... other cases if any
    default: return "Unknown";
    }
}

#endif // OBJECTTYPE_H
