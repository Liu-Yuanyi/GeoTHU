#include "point.h"
#include "objecttype.h"
#include "calculator.h"

Point::Point(const QPointF& position) : GeometricObject(ObjectName::Point), PointArg(position) {
    generation_=0;
    GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
}

Point::Point(const std::vector<GeometricObject*>& parents,const int& generation)
    : GeometricObject(ObjectName::Point){
    for(auto iter: parents){
        addParent(iter);
    }
    generation_=generation;
    GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
    setPosition();
}

void Point::setPosition(const QPointF& pos) {
    switch(generation_){
    case 0:{
        expectParentNum(0);
        PointArg = pos;
        return;
    }
    case 1:
    case 2:
    case 3:{
        expectParentNum(1);
        PointArg.rx()=footRatio(pos,parents_[0]->getTwoPoints(),parents_[0]->getObjectType());
        return;
    }
    case 4:{
        expectParentNum(1);
        PointArg=NearestPointOnCircle(pos,parents_[0]->getTwoPoints())-parents_[0]->getTwoPoints().first;
        return;
    }
    default:
        return;
    };
}

void Point::draw(QPainter* painter) const {
    if (!isShown()) {
        return;
    }

    if (!labelhidden_) {
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
    if(!isShown())return false;
    qreal dx = clickPos.x() - position().x();
    qreal dy = clickPos.y() - position().y();
    return (dx * dx + dy * dy) <= (size_ + 4) * (size_ + 4);
}

void Point::flush(){
    position_.clear();
    legal_=true;
    for(auto iter:parents_){
        if(!iter->isLegal()){
            legal_=false;
            position_.push_back(QPointF());
            return;
        }
    }
    switch(generation_){
    case -4:{
        position_.push_back(2*parents_[1]->position() - parents_[0]->position());
        return;
    }
    case -3:{
        position_.push_back(reflect(parents_[0]->position(),parents_[1]->getTwoPoints()));
        return;
    }
    case 0:{
        position_.push_back(PointArg);
        return;
    }
    case 1:
    case 2:
    case 3:{
        expectParentNum(1);
        auto ppp=parents_[0]->getTwoPoints();
        position_.push_back(ppp.first+PointArg.x()*(ppp.second-ppp.first));
        return;
    }
    case 4:{
        expectParentNum(1);
        auto ppp=parents_[0]->getTwoPoints();
        position_.push_back(ppp.first+PointArg*len(ppp.second-ppp.first)/len(PointArg));
        return;
    }
    case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:{
        expectParentNum(2);
        int range1=(generation_-5)/3,range2=(generation_-5)%3;
        auto res=linelineintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());

        if(
            range1==1&&res.t[0]<0 || range1==2&&(res.t[0]<0||res.t[0]>1)
            ||  range2==1&&res.t[1]<0 || range2==2&&(res.t[1]<0||res.t[1]>1)){
            legal_=false;
        }
        position_.push_back(res.p);
        return;
    }
    case 14: case 15: case 16: case 17: case 18: case 19:{
        expectParentNum(2);
        int range=(generation_-14)/2;
        auto res=linecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false || range==1&&res.t[generation_%2]<0 || range==2&&(res.t[generation_%2]<0||res.t[generation_%2]>1)){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return;
    }
    case 20: case 21:{
        expectParentNum(2);
        auto res=circlecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return;
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
        position_.push_back(QPointF((P1.x() + P2.x()) / 2, (P1.y() + P2.y()) / 2));
        return;
    }
    case 31:case 32:{
        const QPointF& center = parents_[1]->getTwoPoints().first;
        const QPointF& pointOnCircle = parents_[1]->getTwoPoints().second;
        const QPointF& A =parents_[0]->position();
        qreal radius = len(center-pointOnCircle);
        QPointF AC = A - center;
        qreal dist_squared = AC.x() * AC.x() + AC.y() * AC.y();
        qreal dist = std::sqrt(dist_squared);

        if (dist <= radius) {
            legal_=false;
            position_.push_back(QPointF());
            return;
        }

        qreal h = std::sqrt(dist_squared - radius * radius);
        qreal cos_theta = radius / dist;
        qreal sin_theta = h / dist;

        if(generation_==31) {
            position_.push_back(QPointF(
                center.x() + (AC.x() * cos_theta - AC.y() * sin_theta) * radius / dist,
                center.y() + (AC.x() * sin_theta + AC.y() * cos_theta) * radius / dist
                ));
        } else {
            position_.push_back(QPointF(
                center.x() + (AC.x() * cos_theta + AC.y() * sin_theta) * radius / dist,
                center.y() + (-AC.x() * sin_theta + AC.y() * cos_theta) * radius / dist
                ));
        }
        return;
    }
    default:
        QMessageBox::warning(nullptr, "警告", "Point的flush方法没有完成!");
        position_.push_back(QPointF());
        return;
    };
}

QPointF Point::position() const{
    return position_[0];
}
