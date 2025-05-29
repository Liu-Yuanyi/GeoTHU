#ifndef GEOMETRICOBJECT_H
#define GEOMETRICOBJECT_H

#include <QPainter>
#include <vector>
#include "objecttype.h"
#include<qmessagebox.h>

extern std::map<ObjectType, QString> GetDefaultLable;
extern std::map<ObjectType, QColor> GetDefaultColor;
extern std::map<ObjectType, double> GetDefaultSize;
extern std::map<ObjectType, int> GetDefaultShape;
namespace LineStyle {
const int Solid = 0;
const int Dashed = 1;
const int Dotted = 2; // 如果以后需要
// ... 其他样式
}

class GeometricObject {
public:

    GeometricObject(ObjectName name);

    virtual ~GeometricObject();

    virtual ObjectType getObjectType() const = 0;
    virtual void draw(QPainter* painter) const = 0;
    virtual bool isNear(const QPointF& Pos) const = 0;
    virtual QPointF position() const = 0;
    virtual std::pair<const QPointF, const QPointF> getTwoPoints() const;

    // --- Status Getters ---
    bool isShown()const {return legal_ && !hidden_;}
    bool isSelected() const { return selected_; }
    bool isHidden() const { return hidden_; }
    bool isLegal() const { return legal_; }
    bool isHovered() const { return hovered_; }
    QString getLabel() const { return label_; }
    QColor getColor() const { return color_; }
    double getSize() const { return size_; }
    int getShape() const { return shape_; }
    bool islablehidden() const {return labelhidden_;}

    // --- Status Setters ---
    void setSelected(bool selected) { selected_ = selected; }
    void setHidden(bool hidden) { hidden_ = hidden; }
    void setLegal(bool legal) { legal_ = legal ;}
    void setHovered(bool hovered) { hovered_ = hovered; }
    void setlabelhidden(bool labelhidden) { labelhidden_ = labelhidden; }
    void setLabel(QString str) { label_ = str; } // update default label to the next char maybe?
    void setColor(QColor color){ color_ = color; GetDefaultColor[name_] = color; }
    void setSize(double size) { size_ = size; GetDefaultSize[name_] = size; }
    void setShape(int shape) { shape_ = shape; GetDefaultShape[name_] = shape;}

    // --- Parent Management ---
    bool addParent(GeometricObject* parent);//四个add/remove函数都内置了双向设置, 也就是只要A设置add/remove B, B自动就会add/remove A
    bool removeParent(GeometricObject* parent);
    const std::vector<GeometricObject*>& getParents() const;
    bool hasParent(GeometricObject* parent) const;

    // --- Child Management ---
    bool addChild(GeometricObject* child);
    bool removeChild(GeometricObject* child);
    const std::vector<GeometricObject*>& getChildren() const;
    bool hasChild(GeometricObject* child) const;

    virtual GeometricObject* flush()=0;//返回自己
    virtual bool isTouchedByRectangle(const QPointF& start, const QPointF& end) const=0;

protected:

    inline void expectParentNum(size_t num)const{
        if(parents_.size()!=num)
            QMessageBox::warning(
                nullptr,
                "警告",
                label_+"的parents_大小不为"+QString::number(num)+"!");
    }
    std::vector<QPointF> position_;
    bool selected_;
    bool hovered_;
    mutable bool legal_;
    bool hidden_;
    bool labelhidden_;
    std::vector<GeometricObject*> parents_ = {};
    std::vector<GeometricObject*> children_ = {};
    QString label_;
    QColor color_;
    double size_;
    int shape_;
    int generation_;//这个对象是怎么产生的
    //统一约定: -1为平移产生的, -2为旋转产生的, -3为轴对称产生的, -4为中心对称产生的, -5为反演产生的
    ObjectName name_;
};

inline QString nextLineLable(const QString& input) {
    // 从后往前找到第一个非数字字符的位置
    int i = input.length();
    while (i > 0 && input[i-1].isDigit()) {
        --i;
    }

    // 如果没有数字部分，直接返回原字符串
    if (i == input.length()) {
        return input;
    }

    // 分离单词和数字部分
    QString word = input.left(i);
    QString numStr = input.mid(i);

    // 将数字部分转为整数并加1
    bool ok;
    int num = numStr.toInt(&ok);
    if (!ok) {
        return input;  // 转换失败时返回原字符串
    }

    // 拼接结果
    return word + QString::number(num + 1);
}

inline QString nextPointLable(const QString& input) {
    QString result = input;

    // 从字符串的最后一个字符开始向前处理
    for (int i = result.size() - 1; i >= 0; --i) {
        QChar& c = result[i];

        // 如果当前字符不是'Z'，直接加1并返回
        if (c != 'Z') {
            c = QChar(c.unicode() + 1);
            return result;
        }

        // 如果当前字符是'Z'，将其置为'A'，并继续处理前一位
        c = 'A';
    }

    // 如果所有字符都是'Z'，则需要在前面添加一个'A'
    result.prepend('A');
    return result;
}

#endif // GEOMETRICOBJECT_H
