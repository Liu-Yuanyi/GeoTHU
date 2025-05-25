#include "point.h"
#include "objecttype.h"
#include "intersectioncalculator.h"
inline const qreal footRatio(QPointF P, std::pair<QPointF, QPointF> line, ObjectType mode=ObjectType::Line) {
    const QPointF& A = line.first;
    const QPointF& B = line.second;
    // 计算线段AB的向量
    QPointF AB = B - A;
    // 计算点P到点A的向量
    QPointF AP = P - A;
    // 计算AB的长度平方
    qreal abLengthSquared = AB.x() * AB.x() + AB.y() * AB.y();
    // 如果线段长度为0，返回端点A
    if (abLengthSquared == 0) return 0;
    // 计算点积 AP·AB
    qreal dotProduct = AP.x() * AB.x() + AP.y() * AB.y();
    // 计算投影比例 t
    qreal t;
    if(mode==ObjectType::Lineoo){
        return qBound(0.0, dotProduct / abLengthSquared, 1.0);
    }
    else if(mode == ObjectType::Lineo){
        return qMax(0.0, dotProduct / abLengthSquared);
    }

    else return dotProduct / abLengthSquared;
}

inline const QPointF NearestPointOnCircle(QPointF P, std::pair<QPointF, QPointF> Cir) {
    const QPointF& center = Cir.first;   // 圆心
    const QPointF& onCircle = Cir.second; // 圆上一点，用于确定半径向量

    // 计算半径向量（圆心到圆上点的方向）
    QPointF radiusVector = onCircle - center;
    qreal radius2 = radiusVector.x()*radiusVector.x()+radiusVector.y()*radiusVector.y(); // 半径长度

    // 若半径为0（退化为点），返回圆心
    if (radius2 == 0) return center;

    // 计算点P到圆心的向量
    QPointF toP = P - center;
    qreal disP= toP.x()*toP.x()+toP.y()*toP.y();

    QPointF delta = toP*(sqrt(radius2/disP));

    return center+delta;
}

Point::Point(const QPointF& position) : GeometricObject(ObjectName::Point), position_(position) {
    generation_=0;
    GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
}

Point::Point(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Point){
    parents_=parents;
    generation_=generation;
    GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
    setPosition();
}

void Point::setPosition(const QPointF& pos) {
    switch(generation_){
    case 0:{
        expectParentNum(0);
        position_ = pos;
        return;
    }
    case 1:
    case 2:
    case 3:{
        expectParentNum(1);
        position_.rx()=footRatio(pos,parents_[0]->getTwoPoints(),parents_[0]->getObjectType());
        return;
    }
    case 4:{
        expectParentNum(1);
        position_=NearestPointOnCircle(pos,parents_[0]->getTwoPoints())-parents_[0]->getTwoPoints().first;
        return;
    }
    default:
        return;
    };
}

void Point::draw(QPainter* painter) const {
    if (hidden_) {
        return;
    }

    if (label_ != "") {
        painter->setPen(Qt::black);
        painter->drawText(position().x() + 6, position().y() - 6, label_);
    }

    if (hovered_) {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::black);
        painter->drawEllipse(position(), size_ + 1,  size_ + 1);
        if (selected_){
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(Qt::darkRed, 2));
            painter->drawEllipse(position(), size_ + 3, size_ + 3);
        }
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);    // Smooth edges
    painter->setBrush(color_); // Fill color
    painter->setPen(Qt::black); // Border color
    painter->drawEllipse(position(), size_, size_);

    if (selected_) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::darkRed, 2));
        painter->drawEllipse(position(), size_ + 2, size_ + 2);
    }

    painter->restore();
}

bool Point::isNear(const QPointF& clickPos) const {
    qreal dx = clickPos.x() - position().x();
    qreal dy = clickPos.y() - position().y();
    return (dx * dx + dy * dy) <= (size_ + 2) * (size_ + 2);
}

QPointF Point::position() const{
    legal_=true;
    for(auto iter:parents_){
        if(!iter->isLegal()){
            legal_=false;
            return QPointF();
        }
    }
    switch(generation_){
    case 0:{
        return position_;
    }
    case 1:
    case 2:
    case 3:{
        expectParentNum(1);
        auto ppp=parents_[0]->getTwoPoints();
        return ppp.first+position_.x()*(ppp.second-ppp.first);
    }
    case 4:{
        expectParentNum(1);
        auto ppp=parents_[0]->getTwoPoints();
        return ppp.first+position_*len(ppp.second-ppp.first)/len(position_);
    }
    case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:{
        expectParentNum(2);
        int range1=(generation_-5)/3,range2=(generation_-5)%3;//两根line的限度
        auto res=linelineintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());

        if(
            range1==1&&res.t[0]<0 || range1==2&&(res.t[0]<0||res.t[0]>1)
            ||  range2==1&&res.t[1]<0 || range2==2&&(res.t[1]<0||res.t[1]>1)){
            legal_=false;
        }
        return res.p;
    }
    case 14: case 15: case 16: case 17: case 18: case 19:{
        expectParentNum(2);
        int range=(generation_-14)/2;
        auto res=linecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false || range==1&&res.t[generation_%2]<0 || range==2&&(res.t[generation_%2]<0||res.t[generation_%2]>1)){
            legal_=false;
        }
        return res.p[generation_%2];
    }
    case 20: case 21:{
        expectParentNum(2);
        auto res=circlecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false){
            legal_=false;
        }
        return res.p[generation_%2];
    }
    case 30:{
        QPointF P1, P2;
        if(parents_[0]->getObjectType()==ObjectType::Point){
            expectParentNum(2);
            P1=parents_[0]->position();
            P2=parents_[1]->position();
        } else {
            expectParentNum(1);
            auto p = parents_[0]->getTwoPoints();
            P1 = p.first;
            P2 = p.second;
        }
        return QPointF((P1.x() + P2.x()) / 2, (P1.y() + P2.y()) / 2);
    }
    default:
        return QPointF();
    };
}
