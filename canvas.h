#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QMenu>
#include <QColorDialog>
#include <QInputDialog>
#include <vector>
#include <set>
#include <map>
#include "geometricobject.h"
#include "point.h"
#include "operation.h"

class Canvas : public QWidget {
    Q_OBJECT
public:
    enum Mode { SelectionMode, CreatePointMode, OperationMode, DeletionMode };
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas() override;
    void setMode(Mode newMode);
    void setOperation(const int index);
    void deleteObjects();
    void hideObjects();
    void showObjects();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    std::vector<Operation*> operations;     // 所有可能的 operation
    std::vector<GeometricObject*> objects_; // 存储所有几何对象
    Mode currentMode = SelectionMode;       // 当前画布模式
    std::vector<GeometricObject*> hoveredObjs_ = {}; // 当前鼠标悬停的对象
    GeometricObject* draggedObj_ = nullptr; // 当前拖拽的对象 (在你的代码中似乎主要通过 selectedObjs_ 和 initialPositions_ 实现拖拽)
    std::set<GeometricObject*> selectedObjs_ = {}; // 当前选中的对象集合
    QPointF mousePos_;                      // 鼠标按下时的位置，用于计算拖动偏移
    std::map<GeometricObject*, QPointF> initialPositions_; // 拖动开始时选中对象的初始位置
    bool deselectPermitted_ = true;          // 是否允许在鼠标释放时取消选择（用于区分拖拽和单击）
    Operation* currentOperation_ = nullptr;  // 当前进行的操作 (例如平移、旋转等)
    std::vector<GeometricObject*> operationSelections_; // 记录目前选择了哪些对象

    // --- 私有辅助函数 ---
    void updateHoverState(const QPointF& pos);                  // 更新鼠标悬停状态
    GeometricObject* findObjNear(const QPointF& pos) const;     // 查找指定位置附近的对象
    std::vector<GeometricObject*> findObjectsNear(const QPointF& pos) const;
    Point* findPointNear(const QPointF& pos) const;           // 查找指定位置附近的点对象
    void clearSelections();                                     // 清除所有对象的选中状态
    GeometricObject* automaticIntersection();
};

#endif // CANVAS_H
