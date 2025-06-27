#include "measurement.h"
#include "lineoo.h"
#include <QString>
#include "calculator.h"
int NumOfMeasurements = 0;

constexpr long double TextHeight = 30.0L;
const QFont font("Arial", 16);
constexpr int Precision = 2;

Measurement::Measurement(const std::vector<GeometricObject*>& parents, const int& generation)
    :GeometricObject(ObjectName::Measurement)
{
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    GetDefaultLable[ObjectType::Lineoo]=nextLineLable(GetDefaultLable[ObjectType::Lineoo]);
    NumOfMeasurements++;
    id_=NumOfMeasurements;
}

void Measurement::draw(QPainter* painter) const {
    if (!isShown()) {
        return;
    }

    painter->save(); // 保存当前 painter 状态

    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(text_);
    const int x = 5;
    const int y = id_ * TextHeight;

    // 启用抗锯齿
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setFont(font);

    // 绘制选中状态下的背景
    if (isSelected()) {
        // 梅红色背景 (RGB: 255, 20, 147)
        QBrush backgroundBrush(QColor(255, 20, 147, 128)); // 半透明效果
        painter->fillRect(x, y - fm.ascent(), textRect.width(), textRect.height(), backgroundBrush);
    }
    if(isHovered()) {
        const int borderPadding = 2; // 边框与文本的间距
        QPen hoverPen(Qt::red);
        hoverPen.setWidth(1);
        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush); // 不填充
        painter->drawRect(
            x - borderPadding,
            y - fm.ascent() - borderPadding,
            textRect.width() + 2 * borderPadding,
            textRect.height() + 2 * borderPadding
            );
    }

    // 绘制文本
    painter->setPen(Qt::black);
    painter->drawText(x, y, text_);

    painter->restore(); // 恢复 painter 状态
}

bool Measurement::isNear(const QPointF& pos) const {
    if (!isShown()) return false; // 如果对象隐藏，则认为不在附近
    auto [p1,p2] = getTwoPoints();
    return pos.x()>=p1.x() && pos.x()<=p2.x() && pos.y()>=p1.y() && pos.y()<=p2.y();
}

QPointF Measurement::position() const{
    return QPointF(5,id_ * TextHeight);
}

std::pair<const QPointF, const QPointF> Measurement::getTwoPoints() const {
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(text_);
    const int x = 5;
    const int y = id_ * TextHeight;
    return std::make_pair(QPointF(x, y - fm.ascent()), QPointF(x + textRect.width(), y - fm.ascent() + textRect.height()));
}

GeometricObject* Measurement::flush() {
    legal_ = true;
    for (auto iter : parents_) {
        if (!iter->isLegal()) {
            legal_ = false;
            text_ = "Invalid measurement";
            return this;
        }
    }

    switch (generation_) {
    case 0: { // 长度度量
        if (parents_.size() == 1 && parents_[0]->getObjectType() == ObjectType::Lineoo) {
            Lineoo* segment = dynamic_cast<Lineoo*>(parents_[0]);
            text_.clear();
            text_+=" ";
            text_+=segment->getLabel();
            text_+=" = ";
            text_+=QString::number(segment->length(),'f', Precision );
        }
        else if(parents_.size() == 2 && parents_[0]->getObjectType() == ObjectType::Point &&parents_[1]->getObjectType()==ObjectType::Point) {
            text_.clear();
            text_+=" ";
            text_+=parents_[0]->getLabel();
            text_+=parents_[1]->getLabel();
            text_+=" = ";
            text_+=QString::number(len(parents_[0]->position()-parents_[1]->position()),'f',Precision);
        }
        else{
            QMessageBox::warning(nullptr, "警告", "Measurement的长度测量parents_出错!");
        }
        return this;
    }
    case 1: { // 角度度量
        expectParentNum(3);
        text_.clear();
        text_+=" ";
        text_+="∠";
        text_+=parents_[0]->getLabel();
        text_+=parents_[1]->getLabel();
        text_+=parents_[2]->getLabel();

        text_+=" = ";
        text_+=QString::number(
            PI-abs(normalizeAngle(Theta(parents_[0]->position()-parents_[1]->position())-Theta(parents_[2]->position()-parents_[1]->position()))-PI) ,'f',Precision);
        return this;
    }
    default:
        QMessageBox::warning(nullptr, "警告", "Measurement的flush方法没有完成!");
        text_ = "Invalid generation type";
        return this;
    }
}

bool Measurement::isTouchedByRectangle(const QPointF& start, const QPointF& end) const {
    auto p = getTwoPoints();
    long double x1 = p.first.x(), x2 = p.second.x();
    long double y1 = p.first.y(), y2 = p.second.y();
    long double largerX = std::max(start.x(), end.x()), smallerX = std::min(start.x(), end.x());
    long double largerY = std::max(start.y(), end.y()), smallerY = std::min(start.y(), end.y());

    return !((x1 > largerX && x2 > largerX) || (x1 < smallerX && x2 < smallerX) ||
             (y1 > largerY && y2 > largerY) || (y1 < smallerY && y2 < smallerY));
}

LengthMeasurementCreator::LengthMeasurementCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point});
    inputType.push_back(std::vector<ObjectType>{ObjectType::Lineoo});
    operationName="LengthMeasurementCreator";
    waitImplemented = false;
}

std::set<GeometricObject*> LengthMeasurementCreator::apply(std::vector<GeometricObject*> objs,
                                                            QPointF position) const{
    return std::set<GeometricObject*>{(new Measurement(objs,0))->flush()};
}

AngleMeasurementCreator::AngleMeasurementCreator(){
    inputType.push_back(std::vector<ObjectType>{ObjectType::Point,ObjectType::Point,ObjectType::Point});
    operationName="AngleMeasurementCreator";
    waitImplemented = false;
}

std::set<GeometricObject*> AngleMeasurementCreator::apply(std::vector<GeometricObject*> objs,
                                                            QPointF position) const{
    return std::set<GeometricObject*>{(new Measurement(objs,1))->flush()};
}

