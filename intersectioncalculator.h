#ifndef INTERSECTIONCALCULATOR_H
#define INTERSECTIONCALCULATOR_H

#include"point.h"

struct linelineIntersectionResult{
    QPointF p;
    double t[2];//占两个线段的比例
    bool exist;
};

struct linecircleIntersectionResult{
    QPointF p[2];//前面那个是generation_小的
    double t[2];//两个点在线端上的比例
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
    double denominator = r.x() * s.y() - r.y() * s.x();

    // 如果分母为0，表示线段平行或共线
    if (qFuzzyIsNull(denominator)) {
        return ret;
    }

    // 计算分子
    QPointF AC = C - A;
    double t_numerator = AC.x() * s.y() - AC.y() * s.x();
    double u_numerator = AC.x() * r.y() - AC.y() * r.x();

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
    double radius = len(R - O);

    // 使用 A - O 形成正确的一元二次方程：|A + t(B-A) - O|^2 = r^2
    QPointF dir = B - A;
    QPointF AO = A - O;
    double a = QPointF::dotProduct(dir, dir);
    double b = 2.0 * QPointF::dotProduct(dir, AO);
    double c = QPointF::dotProduct(AO, AO) - radius * radius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return result;
    }

    double sqrtDiscriminant = std::sqrt(discriminant);
    double t1 = (-b + sqrtDiscriminant) / (2.0 * a);
    double t2 = (-b - sqrtDiscriminant) / (2.0 * a);

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
    double r1 = QLineF(A, B).length();

    QPointF O = OR.first;
    QPointF R = OR.second;
    double r2 = QLineF(O, R).length();

    // 计算圆心距
    QPointF AO = O - A;
    double d = len(AO);

    // 处理两圆位置关系
    if (d > r1 + r2 || d < qAbs(r1 - r2)) {
        // 两圆相离或内含，无交点
        return result;
    }

    // 计算辅助变量
    double a = (r1*r1 - r2*r2 + d*d) / (2.0 * d);
    double h_squared = r1*r1 - a*a;

    // 计算交点
    double h = std::sqrt(h_squared);
    QPointF N = AO / d; // 归一化向量
    QPointF T(-N.y(), N.x()); // 垂直向量，顺时针旋转90度

    // 计算两个交点
    QPointF p = A + a * N;
    result.p[0] = p + h * T;
    result.p[1] = p - h * T;

    // 确保向量p1p2和向量AO的叉乘垂直于屏幕朝内
    QPointF p1p2 = result.p[1] - result.p[0];
    double cross = AO.x() * p1p2.y() - AO.y() * p1p2.x();

    result.exist=true;
    return result;
}

#endif // INTERSECTIONCALCULATOR_H
