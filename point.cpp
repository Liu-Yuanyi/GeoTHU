#include "point.h"
#include "objecttype.h"
#include "calculator.h"
#include "circle.h"

Point::Point(const QPointF& position, bool isTemp) : GeometricObject(ObjectName::Point), PointArg(position) {
    generation_=0;
    if (!isTemp) {
        GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
    }
}

Point::Point(const std::vector<GeometricObject*>& parents, const int& generation, bool aux)
    : GeometricObject(ObjectName::Point, aux){
    PointArg = QPoint(1,0);
    for(auto iter: parents){
        addParent(iter);
    }
    generation_ = generation;
    if (!aux){
        GetDefaultLable[ObjectType::Point]=nextPointLable(GetDefaultLable[ObjectType::Point]);
    }
}


Point::~Point(){
    //GetDefaultLable[ObjectType::Point] = previousPointLable(GetDefaultLable[ObjectType::Point]);
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
        PointArg=NearestPointOnCircle(pos,parents_[0]->getTwoPoints())-parents_[0]->position();
        return;
    }
    case 28:{
        expectParentNum(1);
        QPointF tmp=NearestPointOnCircle(pos,parents_[0]->getTwoPoints())-parents_[0]->position();
        long double theta=Theta(tmp);
        auto [s,t]=dynamic_cast<Arc*>(parents_[0])->getAngles();
        if(thetainst(theta, s, t)){
            PointArg.rx()= AngleSubstract(theta,s)/AngleSubstract(t,s);
            return;
        }
        if(AngleSubstract(s,theta)>AngleSubstract(theta,t)){
            PointArg.rx()=1.0L;
            return;
        }
        PointArg.rx()=0.0L;
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
    qDebug()<<position_[0];
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

GeometricObject* Point::flush(){
    position_.clear();
    legal_=true;
    for(auto iter:parents_){
        if(!iter->isLegal()){
            legal_=false;
            position_.push_back(QPointF());
            return this;
        }
    }
    switch(generation_){
    case -4:{
        position_.push_back(2*parents_[1]->position() - parents_[0]->position());
        return this;
    }
    case -3:{
        position_.push_back(reflect(parents_[0]->position(),parents_[1]->getTwoPoints()));
        return this;
    }
    case 0:{
        position_.push_back(PointArg);
        return this;
    }
    case 1:
    case 2:
    case 3:{
        expectParentNum(1);
        auto ppp=parents_[0]->getTwoPoints();
        position_.push_back(ppp.first+PointArg.x()*(ppp.second-ppp.first));
        return this;
    }
    case 4:{
        expectParentNum(1);
        auto ppp=parents_[0]->getTwoPoints();
        position_.push_back(ppp.first+PointArg*len(ppp.second-ppp.first)/len(PointArg));
        return this;
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
        return this;
    }
    case 14: case 15: case 16: case 17: case 18: case 19:{
        expectParentNum(2);
        int range=(generation_-14)/2;
        auto res=linecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false || range==1&&res.t[generation_%2]<0 || range==2&&(res.t[generation_%2]<0||res.t[generation_%2]>1)){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return this;
    }
    case 20: case 21:{
        expectParentNum(2);
        auto res=circlecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return this;
    }
    case 28:{
        expectParentNum(1);
        auto [s,t]=dynamic_cast<Arc*>(parents_[0])->getAngles();
        long double theta = s+ PointArg.x()*AngleSubstract(t,s);
        position_.push_back(parents_[0]->position() + UnitVector(theta)*len(parents_[0]->getTwoPoints()));
        return this;
    }
    case 29:{
        expectParentNum(3);
        position_.push_back(parents_[0]->position() + (parents_[2]->position()-parents_[0]->position())*
                                                        len(parents_[1]->position()-parents_[0]->position())/len(parents_[2]->position()-parents_[0]->position()));
        return this;
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
        return this;
    }
    case 31:case 32:{
        auto [center, pointOnCircle] = parents_[1]->getTwoPoints();
        const QPointF& A =parents_[0]->position();
        qreal radius = len(center-pointOnCircle);
        QPointF AC = A - center;
        qreal dist_squared = AC.x() * AC.x() + AC.y() * AC.y();
        qreal dist = std::sqrt(dist_squared);

        if (dist <= radius) {
            legal_=false;
            position_.push_back(QPointF());
            return this;
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
        if(parents_[1]->getObjectType()==ObjectType::Arc and !thetainst(Theta(position_[0]-center),dynamic_cast<Arc*>(parents_[1])->getAngles())){
            legal_=false;
        }
        return this;
    }
    case 34:case 35:case 36:case 37:case 38:case 39:{
        expectParentNum(2);
        int range=(generation_-34)/2;
        auto res=linecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if( res.exist==false ||
            range==1&&res.t[generation_%2]<0 ||
            range==2&& (res.t[generation_%2]<0 ||res.t[generation_%2]>1) ||
            !thetainst(Theta(res.p[generation_%2]-parents_[1]->position()),
                        dynamic_cast<Arc*>(parents_[1])->getAngles())){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return this;
    }
    case 40:case 41:{
        expectParentNum(2);
        auto res=circlecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false ||
            !thetainst(Theta(res.p[generation_%2]-parents_[1]->position()),
                       dynamic_cast<Arc*>(parents_[1])->getAngles())){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return this;
    }
    case 42:case 43:{
        expectParentNum(2);
        auto res=circlecircleintersection(parents_[0]->getTwoPoints(),parents_[1]->getTwoPoints());
        if(res.exist==false ||
            !thetainst(Theta(res.p[generation_%2]-parents_[1]->position()),
                       dynamic_cast<Arc*>(parents_[1])->getAngles()) ||
            !thetainst(Theta(res.p[generation_%2]-parents_[0]->position()),
                       dynamic_cast<Arc*>(parents_[0])->getAngles())){
            legal_=false;
        }
        position_.push_back(res.p[generation_%2]);
        return this;
    }
    default:
        QMessageBox::warning(nullptr, "警告", "Point的flush方法没有完成!");
        position_.push_back(QPointF());
        return this;
    };
}

QPointF Point::position() const{
    return position_[0];
}

bool Point::isTouchedByRectangle(const QPointF& start, const QPointF& end) const{
    long double x = position().x(), y = position().y();
    long double xStart = start.x(), yStart = start.y();
    long double xEnd = end.x(), yEnd = end.y();
    return (x-xStart) * (x-xEnd) <= 0 and (y-yStart) * (y-yEnd) <= 0;
}
