#ifndef INTERSECTIONCALCULATOR_H
#define INTERSECTIONCALCULATOR_H

#include"point.h"

struct linelineIntersectionResult{
    QPointF p;
    double t1,t2;
    bool legal;
};

struct linecircleIntersectionResult{
    QPointF p1,p2;
    double t1,t2;
    bool legal;
};

struct circlecircleIntersectionResult{
    QPointF p1,p2;
    bool legal;
};

inline linelineIntersectionResult linelineintersection(
    const std::pair<QPointF, QPointF>& AB,
    const std::pair<QPointF, QPointF>& CD)
{
    linelineIntersectionResult ret{QPointF(), 0.0, 0.0, false};

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
    ret.t1 = t_numerator / denominator;
    ret.t2 = u_numerator / denominator;
    ret.p = A + r * ret.t1;
    ret.legal=true;
    return ret;
}

inline linecircleIntersectionResult linecircleintersection(
    const std::pair<QPointF, QPointF>& AB,
    const std::pair<QPointF, QPointF>& OR)
{
    linecircleIntersectionResult result = {QPointF(), QPointF(), 0.0, 0.0, false};

    // 提取直线AB的端点
    QPointF A = AB.first;
    QPointF B = AB.second;

    // 提取圆心O和半径向量R
    QPointF O = OR.first;
    QPointF R = OR.second;
    double radius = len(R-O);

    // 计算直线参数方程 P = A + t(B-A) 中的向量
    QPointF dir = B - A;
    QPointF AO = O - A;

    // 计算参数方程中的系数
    double a = QPointF::dotProduct(dir, dir);
    double b = 2.0 * QPointF::dotProduct(AO, dir);
    double c = QPointF::dotProduct(AO, AO) - radius * radius;

    // 计算判别式
    double discriminant = b * b - 4 * a * c;

    // 处理判别式的情况
    if (discriminant < 0) {
        // 无交点
        return result;
    }

    double sqrtDiscriminant = std::sqrt(discriminant);

    // 计算参数t1和t2
    double t1 = (-b + sqrtDiscriminant) / (2.0 * a);
    double t2 = (-b - sqrtDiscriminant) / (2.0 * a);

    // 计算交点坐标
    result.p1 = A + t1 * dir;
    result.p2 = A + t2 * dir;

    // 存储参数值
    result.t1 = t1;
    result.t2 = t2;

    // 判断是否有有效交点（至少有一个交点在直线上）
    result.legal = true;
    return result;
}

inline circlecircleIntersectionResult circlecircleintersection(
    const std::pair<QPointF, QPointF>& AB,
    const std::pair<QPointF, QPointF>& OR)
{
    circlecircleIntersectionResult result = {QPointF(), QPointF(), false};

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
    result.p1 = p + h * T;
    result.p2 = p - h * T;

    // 确保向量p1p2和向量AO的叉乘垂直于屏幕朝外
    QPointF p1p2 = result.p2 - result.p1;
    double cross = AO.x() * p1p2.y() - AO.y() * p1p2.x();

    if (cross < 0) {
        // 如果叉乘方向相反，则交换p1和p2
        qDebug()<<"圆圆交点交换!";
        std::swap(result.p1, result.p2);
    }
    else{
        qDebug()<<"圆圆交点不换!";
    }
    result.legal=true;
    return result;
}

#endif // INTERSECTIONCALCULATOR_H
