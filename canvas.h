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
#include "circle.h" // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：包含 Circle 头文件
#include "operation.h"

class Canvas : public QWidget {
    Q_OBJECT
public:
    // Mode 枚举中添加 CreateCircleMode 用于创建圆的模式
    enum Mode { SelectionMode, CreatePointMode, CreateCircleMode, OperationMode }; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 修改：添加创建圆模式
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas() override {
        qDeleteAll(objects_); // qDeleteAll 适用于 QObject* 列表，如果 objects_ 是 GeometricObject* 且它们不是 QObject，则需要手动循环删除
            // 或者如果 GeometricObject 的析构是虚的，并且它们是在堆上创建的，则应使用 std::for_each 和 delete
            // 鉴于你之前的代码是 qDeleteAll(objects_)，我假设 GeometricObject* 可以这样清理，或者你需要调整
            // 更安全的做法是： for(GeometricObject* obj : objects_) delete obj; objects_.clear();
        delete previewCircle_; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：清理预览圆对象
    }
    void setMode(Mode newMode);
    void setOperation(Operation* operation);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    std::vector<GeometricObject*> objects_; // 存储所有几何对象
    Mode currentMode = SelectionMode;       // 当前画布模式
    GeometricObject* hoveredObj_ = nullptr; // 当前鼠标悬停的对象
    GeometricObject* draggedObj_ = nullptr; // 当前拖拽的对象 (在你的代码中似乎主要通过 selectedObjs_ 和 initialPositions_ 实现拖拽)
    std::set<GeometricObject*> selectedObjs_ = {}; // 当前选中的对象集合
    QPointF mousePos_;                      // 鼠标按下时的位置，用于计算拖动偏移
    std::map<GeometricObject*, QPointF> initialPositions_; // 拖动开始时选中对象的初始位置
    bool deselectPermitted = true;          // 是否允许在鼠标释放时取消选择（用于区分拖拽和单击）
    Operation* currentOperation = nullptr;  // 当前进行的操作 (例如平移、旋转等)

    // --- 新增：用于创建圆的临时变量 ---
    Circle* previewCircle_ = nullptr;   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：用于在创建过程中预览圆
    QPointF circleCreationCenterPos_; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：存储正在创建的圆的圆心坐标
    Point* circleCreationCenterPoint_ = nullptr; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：可选，如果圆心是已存在的点对象

    // --- 私有辅助函数 ---
    void updateHoverState(const QPointF& pos);                  // 更新鼠标悬停状态
    GeometricObject* findObjNear(const QPointF& pos) const;     // 查找指定位置附近的对象
    Point* findPointNear(const QPointF& pos) const;           // 查找指定位置附近的点对象
    void clearSelections();                                     // 清除所有对象的选中状态
};

#endif // CANVAS_H
