#ifndef CALCULATOR_H

#define CALCULATOR_H

#include "point.h"
#include "math.h"

constexpr long double Epsilon=1e-10L;

constexpr long double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170680L;
constexpr long double PI_2 = 1.5707963267948966192313216916397514420985846996875529104874722961539082031431044993140174126710585340L;

inline long double normalizeAngle(long double angle) {
    return (angle < 0) ? angle + 2*PI : (angle>=2*PI? angle-2*PI : angle);
}
inline long double Theta(const long double& x, const long double& y){
    long double tmp = - PI_2 + atan2(x,y);
    return ((tmp < 0) ? tmp+2.0*PI : tmp);
}
inline long double Theta(const std::pair<long double, long double>& p){
    return Theta(p.first,p.second);
}
inline long double Theta(const QPointF& p){
    return Theta(p.x(),p.y());
}
inline long double Theta(const std::pair<QPointF,QPointF> p){
    return Theta(p.first.x()-p.second.x(),p.first.y()-p.second.y());
}

inline bool is0(long double p){
    return -Epsilon<p && p<Epsilon;
}
inline bool isp0(long double p){
    return 0L<=p && p<Epsilon;
}

inline const QPointF calculateCircleCenter(const QPointF& p1, const QPointF& p2, const QPointF& p3) {
    // 计算向量 v1 = (p2 - p1) 和 v2 = (p3 - p1)
    long double x1 = p2.x() - p1.x();
    long double y1 = p2.y() - p1.y();
    long double x2 = p3.x() - p1.x();
    long double y2 = p3.y() - p1.y();

    // 计算三点共线检测的叉积
    long double cross = x1 * y2 - x2 * y1;

    // 处理共线或接近共线的情况
#warning 这里需要修改
    if (std::abs(cross) < 1e-8) {
        cross = 1e-8;
    }

    // 计算垂直平分线交点（圆心）
    long double f = (x1*x1 + y1*y1) / 2.0;
    long double g = (x2*x2 + y2*y2) / 2.0;

    long double denominator = cross;
    long double cx = p1.x() + (f*y2 - g*y1) / denominator;
    long double cy = p1.y() + (g*x1 - f*x2) / denominator;

    return QPointF(cx, cy);
}


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


struct linelineIntersectionResult{
    QPointF p;
    long double t[2];//占两个线段的比例
    bool exist;
};

struct linecircleIntersectionResult{
    QPointF p[2];//前面那个是generation_小的
    long double t[2];//两个点在线端上的比例
    bool exist;
};

struct circlecircleIntersectionResult{
    QPointF p[2];//前面那个是generation_小的
    bool exist;
};

inline linelineIntersectionResult linelineintersection(
    const std::pair<QPointF, QPointF>& AB,
    const std::pair<QPointF, QPointF>& CD)
{
    linelineIntersectionResult ret{QPointF(), {0.0, 0.0}, false};

    const QPointF A = AB.first;
    const QPointF B = AB.second;
    const QPointF C = CD.first;
    const QPointF D = CD.second;

    // 计算向量
    QPointF r = B - A;
    QPointF s = D - C;

    // 计算分母
    long double denominator = r.x() * s.y() - r.y() * s.x();

    // 如果分母为0，表示线段平行或共线
    if (is0(denominator)) {
        return ret;
    }

    // 计算分子
    QPointF AC = C - A;
    long double t_numerator = AC.x() * s.y() - AC.y() * s.x();
    long double u_numerator = AC.x() * r.y() - AC.y() * r.x();

    // 计算比例参数
    ret.t[0] = t_numerator / denominator;
    ret.t[1] = u_numerator / denominator;
    ret.p = A + r * ret.t[0];
    ret.exist=true;
    return ret;
}

inline linecircleIntersectionResult linecircleintersection(
    const std::pair<QPointF, QPointF>& AB,
    const std::pair<QPointF, QPointF>& OR)
{
    linecircleIntersectionResult result = {{QPointF(), QPointF()}, {0.0, 0.0}, false};

    // 直线上两点
    QPointF A = AB.first;
    QPointF B = AB.second;
    // 圆心和圆上某点：用来确定半径
    QPointF O = OR.first;
    QPointF R = OR.second;
    long double radius = len(R - O);

    // 使用 A - O 形成正确的一元二次方程：|A + t(B-A) - O|^2 = r^2
    QPointF dir = B - A;
    QPointF AO = A - O;
    long double a = QPointF::dotProduct(dir, dir);
    long double b = 2.0 * QPointF::dotProduct(dir, AO);
    long double c = QPointF::dotProduct(AO, AO) - radius * radius;

    long double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        long double t=footRatio(O,AB);
        QPointF foot=A+t*(B-A);
        if(is0(len(foot-O)-len(R-O))){
            result.p[0]=result.p[1]=foot;
            result.t[0]=result.t[1]=t;
            result.exist=true;
        }
        return result;
    }

    long double sqrtDiscriminant = std::sqrt(discriminant);
    long double t1 = (-b + sqrtDiscriminant) / (2.0 * a);
    long double t2 = (-b - sqrtDiscriminant) / (2.0 * a);

    // 按 t 值排序，确保 p[0] 对应较小的 t
    if (t1 <= t2) {
        result.t[0] = t1;
        result.t[1] = t2;
        result.p[0] = A + t1 * dir;
        result.p[1] = A + t2 * dir;
    } else {
        result.t[0] = t2;
        result.t[1] = t1;
        result.p[0] = A + t2 * dir;
        result.p[1] = A + t1 * dir;
    }
    result.exist = true;
    return result;
}

inline circlecircleIntersectionResult circlecircleintersection(
    const std::pair<QPointF, QPointF>& AB,
    const std::pair<QPointF, QPointF>& OR)
{
    circlecircleIntersectionResult result = {{QPointF(), QPointF()}, false};

    // 提取圆心和半径
    QPointF A = AB.first;
    QPointF B = AB.second;
    long double r1 = QLineF(A, B).length();

    QPointF O = OR.first;
    QPointF R = OR.second;
    long double r2 = QLineF(O, R).length();

    // 计算圆心距
    QPointF AO = O - A;
    long double d = len(AO);

    // 处理两圆位置关系
    if (d > r1 + r2 || d < qAbs(r1 - r2)) {
        if(isp0(d-r1-r2) || isp0(qAbs(r1-r2)-d)){
            result.p[0]=result.p[1]= (len(A-B)>len(O-R)? A + (len(B-A)/len(O-A))*(O-A):O + (len(R-O)/len(O-A))*(A-O));
            result.exist=true;
        }
        return result;
    }

    // 计算辅助变量
    long double a = (r1*r1 - r2*r2 + d*d) / (2.0 * d);
    long double h_squared = r1*r1 - a*a;

    // 计算交点
    long double h = std::sqrt(h_squared);
    QPointF N = AO / d; // 归一化向量
    QPointF T(-N.y(), N.x()); // 垂直向量，顺时针旋转90度

    // 计算两个交点
    QPointF p = A + a * N;
    result.p[0] = p + h * T;
    result.p[1] = p - h * T;

    // 确保向量p1p2和向量AO的叉乘垂直于屏幕朝内
    QPointF p1p2 = result.p[1] - result.p[0];

    result.exist=true;
    return result;
}

inline QPointF reflect(const QPointF & P, const std::pair<QPointF,QPointF>& line){
    const QPointF& A = line.first;
    const QPointF& B = line.second;

    // 计算线段AB的向量
    QPointF AB = B - A;
    // 计算点P到点A的向量
    QPointF AP = P - A;

    // 计算AB的长度平方
    qreal abLengthSquared = AB.x() * AB.x() + AB.y() * AB.y();

    // 如果线段长度为0，返回点P自身（因为A和B重合，无法定义反射轴）
    if (abLengthSquared == 0) return P;

    // 计算点积 AP·AB
    qreal dotProduct = AP.x() * AB.x() + AP.y() * AB.y();

    // 计算投影比例 t
    qreal t = dotProduct / abLengthSquared;

    // 计算垂足H
    QPointF H = A + t * AB;

    // 计算反射点R：H + (H - P) = 2H - P
    return 2.0 * H - P;
}

#endif // CALCULATOR_H
