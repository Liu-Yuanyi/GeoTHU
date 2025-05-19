#ifndef GEOMETRICOBJECT_CPP
#define GEOMETRICOBJECT_CPP

#include "geometricobject.h"
#include <qmessagebox.h>
// 默认标签映射表
std::map<ObjectType, QString> GetDefaultLable = {
    {ObjectType::Point, "A"},       // 点的默认标签
    {ObjectType::Line, "line_1"},
    {ObjectType::Lineo, "ray_1"},
    {ObjectType::Lineoo, "segment_1"},
    {ObjectType::Circle, "Circle1"}
};

// 默认颜色映射表
std::map<ObjectType, QColor> GetDefaultColor = {
    {ObjectType::Point, Qt::red},
    {ObjectType::Line, Qt::darkBlue},
    {ObjectType::Lineo, Qt::darkBlue},
    {ObjectType::Lineoo, Qt::darkBlue},
    {ObjectType::Circle, Qt::black}
};

// 默认大小/粗细映射表
std::map<ObjectType, double> GetDefaultSize = {
    {ObjectType::Point, 4.0},  // 点的默认大小
    {ObjectType::Line, 2.0},
    {ObjectType::Lineo, 2.0},
    {ObjectType::Lineoo, 2.0},
    {ObjectType::Circle, 2.0}
};

// 默认形状/线型映射表
std::map<ObjectType, int> GetDefaultShape = {
    {ObjectType::Point, LineStyle::Solid},  // 点的默认形状 (通常不适用线型，但为保持一致性)
    {ObjectType::Line, LineStyle::Solid},
    {ObjectType::Lineo, LineStyle::Solid},
    {ObjectType::Lineoo, LineStyle::Solid},
    {ObjectType::Circle, LineStyle::Solid}
};


GeometricObject::GeometricObject(ObjectName name):
    selected_(true),      // 默认选中状态 (注意：这里设置为 true，通常可能希望是 false)
    hovered_(false),      // 默认悬停状态
    legal_(true),         // 默认合法状态
    hidden_(false),       // 默认隐藏状态
    name_(name),          // 对象类型名
    label_(GetDefaultLable[name]),   // 使用映射表获取默认标签
    color_(GetDefaultColor[name]),   // 使用映射表获取默认颜色
    size_(GetDefaultSize[name]),     // 使用映射表获取默认大小
    shape_(GetDefaultShape[name]),   // 使用映射表获取默认形状/线型
    generation_(0) {}     // 对象的生成代数，初始为0

GeometricObject::~GeometricObject() {
    // 移除父子关系
    std::vector<GeometricObject*> parents_copy = parents_; // 创建父对象列表的副本以安全迭代
    for (GeometricObject* p : parents_copy) {
        if (p) {
            p->removeChild(this); // 让父对象移除对当前对象的子对象引用
        }
    }

    std::vector<GeometricObject*> children_copy = children_; // 创建子对象列表的副本以安全迭代
    for (GeometricObject* c : children_copy) {
        if (c) {
            c->removeParent(this); // 让子对象移除对当前对象的父对象引用
        }
    }

    // 删除所有子对象
    // 注意：这里直接 delete 子对象，意味着 GeometricObject 拥有其子对象的所有权。
    // 这在某些设计中是合适的，但也可能导致双重删除，如果子对象也由其他地方管理。
    for (GeometricObject* c : children_) {
        delete c;  // 删除子对象
    }
    children_.clear();  // 清空子对象列表
    parents_.clear();   // 清空父对象列表
}

bool GeometricObject::addParent(GeometricObject* parent) {
    if (!parent || parent == this) {
        return false; // 无效操作：父对象为空或父对象是自身
    }
    // 检查是否已经是父对象
    if (std::find(parents_.begin(), parents_.end(), parent) != parents_.end()) {
        return false; // 已经是父对象，未做更改
    }
    parents_.push_back(parent);
    parent->addChild(this); // 维持双向关系：让父对象也添加当前对象作为子对象
    return true; // 成功添加到当前对象的父对象列表
}

bool GeometricObject::addChild(GeometricObject* child) {
    if (!child || child == this) {
        return false; // 无效操作：子对象为空或子对象是自身
    }
    // 检查是否已经是子对象
    if (std::find(children_.begin(), children_.end(), child) != children_.end()) {
        return false; // 已经是子对象，未做更改
    }
    children_.push_back(child);
    child->addParent(this); // 维持双向关系：让子对象也添加当前对象作为父对象
    return true; // 成功添加到当前对象的子对象列表
}

bool GeometricObject::removeChild(GeometricObject* child) {
    if (!child) {
        return false; // 不能移除空指针
    }
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
        child->removeParent(this); // 维持双向关系：让子对象也移除当前对象的父对象引用
        return true; // 成功从当前对象的子对象列表中移除
    }
    return false; // 未找到子对象，未做更改
}


bool GeometricObject::removeParent(GeometricObject* parent) {
    if (!parent) {
        return false; // 不能移除空指针
    }
    auto it = std::find(parents_.begin(), parents_.end(), parent);
    if (it != parents_.end()) {
        parents_.erase(it);
        parent->removeChild(this); // 维持双向关系：让父对象也移除当前对象的子对象引用
        return true; // 成功从当前对象的父对象列表中移除
    }
    return false; // 未找到父对象，未做更改
}

const std::vector<GeometricObject*>& GeometricObject::getChildren() const {
    return children_; // 返回子对象列表的常量引用
}

const std::vector<GeometricObject*>& GeometricObject::getParents() const {
    return parents_; // 返回父对象列表的常量引用
}

bool GeometricObject::hasChild(GeometricObject* child) const {
    if (!child) return false; // 如果检查的子对象为空，则返回 false
    return std::find(children_.begin(), children_.end(), child) != children_.end(); // 检查是否存在指定的子对象
}

bool GeometricObject::hasParent(GeometricObject* parent) const {
    if (!parent) return false; // 如果检查的父对象为空，则返回 false
    return std::find(parents_.begin(), parents_.end(), parent) != parents_.end(); // 检查是否存在指定的父对象
}

std::pair<const QPointF, const QPointF> GeometricObject::getTwoPoints() const{
    QMessageBox::warning(
        nullptr,
        "警告",
        QString::fromStdString(std::string(GetObjectNameString(this->getObjectType()))+"没有getTwoPoint方法!")
        );
    return std::make_pair(QPointF(),QPointF());
}

#endif
