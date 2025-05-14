#include "canvas.h"
#include <QPainter>
#include <QInputDialog> // 确保包含 QInputDialog
#include <QColorDialog> // 确保包含 QColorDialog
#include <cmath>        // For std::sqrt, std::pow, std::abs (QLineF::length() 也可以)
#include <algorithm>    // For std::remove if deleting objects

// 假设你的 ObjectType 和 ObjectName 在 "objecttype.h" (或其他地方) 定义，并且 GetDefault... 映射存在
// extern std::map<ObjectType, QColor> GetDefaultColor;
// extern std::map<ObjectType, double> GetDefaultSize;
// extern std::map<ObjectType, QString> GetDefaultLable;


Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true); // 开启鼠标跟踪以接收 mouseMoveEvent (即使没有按钮按下)
    currentOperation = nullptr; // 初始化
    previewCircle_ = nullptr;   // 初始化
    circleCreationCenterPoint_ = nullptr; // 初始化
}

void Canvas::setMode(Mode newMode) {
    // 当离开创建圆模式时，清理临时的预览圆对象
    if (currentMode == CreateCircleMode && newMode != CreateCircleMode) {
        delete previewCircle_;
        previewCircle_ = nullptr;
        circleCreationCenterPoint_ = nullptr;
    }
    // 你也可以在这里添加离开其他模式时的清理逻辑

    currentMode = newMode;
    // clearSelections(); // 切换模式时是否清除选择，根据需求决定
    update(); // 更新画布显示，例如模式切换可能有视觉提示
}

void Canvas::setOperation(Operation* operation) {
    this->currentOperation = operation;
}

void Canvas::updateHoverState(const QPointF& pos) {
    GeometricObject* newHover = findObjNear(pos);
    if (newHover != hoveredObj_) {
        if (hoveredObj_) {
            hoveredObj_->setHovered(false);
        }
        hoveredObj_ = newHover;
        if (hoveredObj_) {
            hoveredObj_->setHovered(true);
        }
        update();
    }
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    mousePos_ = event->position(); // 记录鼠标按下位置，主要用于拖拽计算

    if (event->button() == Qt::LeftButton) {
        initialPositions_.clear(); // 清空，为新的拖拽或点选操作做准备

        if (currentMode == SelectionMode) {
            GeometricObject* clickedObj = findObjNear(mousePos_);
            if (clickedObj) { // 点击到了对象
                if (!clickedObj->isSelected()) { // 如果对象未被选中
                    if (!(event->modifiers() & Qt::ControlModifier)) { // 如果没有按下Ctrl键，清除之前的选择
                        clearSelections();
                    }
                    clickedObj->setSelected(true);
                    selectedObjs_.insert(clickedObj);
                } else { // 如果对象已被选中
                    if (event->modifiers() & Qt::ControlModifier) { // 如果按下了Ctrl键，则取消选中该对象 (toggle)
                        clickedObj->setSelected(false);
                        selectedObjs_.erase(clickedObj);
                    }
                    // 如果已选中且未按Ctrl，则保持选中状态，准备拖动
                }
                deselectPermitted = false; // 准备拖动或多选，释放时不取消选择
            } else { // 点击到空白区域
                if (!(event->modifiers() & Qt::ControlModifier)) { // 如果没有按下Ctrl键
                    clearSelections();
                }
                deselectPermitted = true; // 允许在释放时取消选择（如果这是一个简单的点击）
            }
            // 为所有选中的对象记录初始位置，用于拖动
            for (auto obj : selectedObjs_) {
                initialPositions_[obj] = obj->position();
            }
        } else if (currentMode == CreatePointMode) {
            clearSelections(); // 创建新对象前通常清除选择
            Point* existingPoint = findPointNear(mousePos_);
            if (!existingPoint) { // 如果附近没有点，则创建新点
                Point* newPoint = new Point(mousePos_);
                // newPoint->setColor(GetDefaultColor[ObjectType::Point]); // 设置默认颜色
                // newPoint->setSize(GetDefaultSize[ObjectType::Point]);   // 设置默认大小
                objects_.push_back(newPoint);
                newPoint->setSelected(true); // 新创建的点默认为选中状态
                selectedObjs_.insert(newPoint);
            } else { // 如果点击在现有点附近，则选中该点
                existingPoint->setSelected(true);
                selectedObjs_.insert(existingPoint);
            }
            deselectPermitted = false;
        } else if (currentMode == CreateCircleMode) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：创建圆模式下的鼠标按下逻辑
            clearSelections(); // 创建新对象前清除选择
            deselectPermitted = false; // 正在创建，释放时不取消选择

            // 尝试捕捉到附近的现有 Point 对象作为圆心
            circleCreationCenterPoint_ = findPointNear(mousePos_);
            if (circleCreationCenterPoint_) {
                circleCreationCenterPos_ = circleCreationCenterPoint_->position();
                // 可以选择性地也选中圆心点
                // circleCreationCenterPoint_->setSelected(true);
                // selectedObjs_.insert(circleCreationCenterPoint_);
            } else {
                circleCreationCenterPos_ = mousePos_; // 使用当前鼠标位置作为圆心
            }

            // 创建一个预览圆，初始半径很小
            // 确保你的 ObjectName 枚举中有 ObjectName::Circle
            if (circleCreationCenterPoint_) {
                previewCircle_ = new Circle(circleCreationCenterPoint_, 0.1, ObjectName::Circle);
            } else {
                previewCircle_ = new Circle(circleCreationCenterPos_, 0.1, ObjectName::Circle);
            }
            // previewCircle_->setColor(GetDefaultColor[ObjectType::Circle]); // 设置默认颜色
            // previewCircle_->setSize(GetDefaultSize[ObjectType::Circle]);   // 设置默认线宽
            // previewCircle_->setHovered(true); // 可以给预览对象一个高亮状态
        }
        update();
    } else if (event->button() == Qt::RightButton) { // 右键点击
        GeometricObject* clickedObj = findObjNear(mousePos_);
        if (clickedObj) {
            if (!clickedObj->isSelected()) { // 如果右键点击的对象未被选中
                clearSelections(); // 清除其他选择
                clickedObj->setSelected(true); // 选中该对象
                selectedObjs_.insert(clickedObj);
            }
            // 如果对象已被选中，并且有多个对象被选中，则保持当前选择状态，
            // 以便上下文菜单可以作用于所有选中的对象。
            // 这是一个用户体验的选择。
        } else {
            clearSelections(); // 右键点击空白处，清除所有选择
        }
        deselectPermitted = false; // 右键操作后通常不允许立即取消选择
        update();
        // Qt 会自动在之后调用 contextMenuEvent
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentPos = event->position();
    updateHoverState(currentPos); // 实时更新悬停对象

    if (currentMode == SelectionMode) {
        if (!selectedObjs_.empty() && (event->buttons() & Qt::LeftButton)) { // 如果有选中的对象并且按住左键拖动
            QPointF delta = currentPos - mousePos_; // 计算拖动向量
            for (auto obj : selectedObjs_) {
                QPointF newPos = initialPositions_[obj] + delta; // 计算新位置
                if (obj->getObjectType() == ObjectType::Point) {
                    Point* point = dynamic_cast<Point*>(obj);
                    if (point) point->setPosition(newPos);
                } else if (obj->getObjectType() == ObjectType::Circle) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：拖动圆的逻辑
                    Circle* circle = dynamic_cast<Circle*>(obj);
                    if (circle) {
                        // 如果圆的圆心是一个独立的 Point 对象，并且该 Point 对象也被选中并一起拖动，
                        // 那么圆的位置应该通过其父 Point 对象的移动来更新。
                        // 这里处理的是直接拖动圆对象本身（如果它的圆心不是一个独立的、被选中的 Point）。
                        if (!circle->getCenterPoint() || !selectedObjs_.count(circle->getCenterPoint())) {
                            // 假设 Circle 有 setCenterCoordinates 方法
                            circle->setCenterCoordinates(newPos); // newPos 在这里代表圆心的新位置
                        }
                    }
                }
                // 你可能需要为其他类型的对象（如线、多边形等）添加拖动逻辑
            }
            deselectPermitted = false; // 拖动过程中不允许取消选择
            update();
        }
    } else if (currentMode == CreateCircleMode) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：创建圆模式下的鼠标移动逻辑
        if (previewCircle_ && (event->buttons() & Qt::LeftButton)) { // 如果正在创建圆（左键按下并拖动）
            double radius = QLineF(circleCreationCenterPos_, currentPos).length();
            if (radius < 0.5) radius = 0.5; // 设置一个最小半径，避免半径为0
            previewCircle_->setRadius(radius);
            update();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (currentMode == CreateCircleMode) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：创建圆模式下的鼠标释放逻辑
            if (previewCircle_) {
                if (previewCircle_->getRadius() > 0.5) { // 检查半径是否有效
                    // 将预览圆添加到正式的对象列表中
                    objects_.push_back(previewCircle_);
                    // 可以选择在创建后选中这个新圆
                    // clearSelections();
                    // previewCircle_->setSelected(true);
                    // selectedObjs_.insert(previewCircle_);
                    previewCircle_ = nullptr; // 置空预览指针，因为对象已移交给 objects_ 管理
                } else {
                    delete previewCircle_; // 如果半径太小或无效，则删除预览对象
                    previewCircle_ = nullptr;
                }
                circleCreationCenterPoint_ = nullptr; // 重置临时圆心点指针
            }
            // setMode(SelectionMode); // 创建完毕后可以自动切换回选择模式
            deselectPermitted = true; // 重置标志
            update();
        } else if (currentMode == SelectionMode) {
            // 处理拖拽结束后的 deselectPermitted 状态
            if (!deselectPermitted) { // 如果之前是拖拽或按下时选中
                deselectPermitted = true;
            } else { // 如果是一个简单的点击释放（没有拖拽，且按下时未改变选择状态）
                // 这里的逻辑用于处理：如果用户点击一个已选中的对象（非Ctrl），是否取消选择它。
                // 当前的 mousePressEvent 逻辑已经处理了大部分选择情况。
                // 如果需要点击已选中的对象（非Ctrl）来取消选择它，可以在这里添加。
                // GeometricObject* clickedObj = findObjNear(event->position());
                // if (clickedObj && clickedObj->isSelected() && !(event->modifiers() & Qt::ControlModifier)) {
                //    clickedObj->setSelected(false);
                //    selectedObjs_.erase(clickedObj);
                // }
            }
            update();
        } else if (currentMode == CreatePointMode) {
            // 创建点主要在 mousePressEvent 中完成，释放时通常不需要额外操作
            // setMode(SelectionMode); // 可以选择切换回选择模式
            deselectPermitted = true;
            update();
        }
    }
}

void Canvas::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿，使图形更平滑

    // 绘制所有正式的几何对象
    for (const auto* obj : objects_) {
        if (obj && !obj->isHidden()) { // 检查对象是否存在且未隐藏
            obj->draw(&painter);
        }
    }

    // 如果正在创建圆，绘制预览圆
    if (previewCircle_ && !previewCircle_->isHidden()) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：绘制预览圆
        previewCircle_->draw(&painter);
    }

    // 未来可以添加其他预览元素的绘制，如选择框、辅助线等
}

void Canvas::contextMenuEvent(QContextMenuEvent* event) {
    QPointF pos = event->pos(); // 获取鼠标右键点击的画布坐标
    GeometricObject* contextMenuObj = findObjNear(pos); // 查找点击位置的对象

    if (!contextMenuObj) return; // 如果没有找到对象，则不显示菜单

    // 确保右键点击的对象被选中，这是一个常见的用户体验
    if (!contextMenuObj->isSelected()) {
        clearSelections();
        contextMenuObj->setSelected(true);
        selectedObjs_.insert(contextMenuObj);
        update(); // 更新显示以反映选择变化
    }

    QMenu menu(this); // 创建上下文菜单

    // --- 针对不同对象类型的菜单项 ---
    if (contextMenuObj->getObjectType() == ObjectType::Point) {
        Point* point = dynamic_cast<Point*>(contextMenuObj);
        if (!point) return;

        QMenu* colorMenu = menu.addMenu(tr("颜色")); // tr() 用于国际化
        colorMenu->addAction(tr("红色"), [this, point]() { point->setColor(Qt::red); update(); });
        colorMenu->addAction(tr("蓝色"), [this, point]() { point->setColor(Qt::blue); update(); });
        colorMenu->addAction(tr("绿色"), [this, point]() { point->setColor(Qt::green); update(); });
        colorMenu->addAction(tr("黑色"), [this, point]() { point->setColor(Qt::black); update(); });
        colorMenu->addAction(tr("自定义..."), [this, point]() {
            QColor color = QColorDialog::getColor(point->getColor(), this, tr("选择点颜色"));
            if (color.isValid()) {
                point->setColor(color);
                update();
            }
        });

        QMenu* sizeMenu = menu.addMenu(tr("大小"));
        sizeMenu->addAction(tr("极小"), [this, point]() { point->setSize(2); update(); });
        sizeMenu->addAction(tr("小"),   [this, point]() { point->setSize(3); update(); });
        sizeMenu->addAction(tr("中"),   [this, point]() { point->setSize(4); update(); });
        sizeMenu->addAction(tr("大"),   [this, point]() { point->setSize(5); update(); });

        menu.addAction(tr("标签..."), [this, point]() {
            bool ok;
            QString text = QInputDialog::getText(this, tr("设置标签"), tr("标签文本:"), QLineEdit::Normal, point->getLabel(), &ok);
            if (ok) {
                point->setLabel(text);
                update();
            }
        });

    } else if (contextMenuObj->getObjectType() == ObjectType::Circle) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：圆的上下文菜单
        Circle* circle = dynamic_cast<Circle*>(contextMenuObj);
        if (!circle) return;

        QMenu* colorMenu = menu.addMenu(tr("线条颜色"));
        colorMenu->addAction(tr("红色"), [this, circle]() { circle->setColor(Qt::red); update(); });
        colorMenu->addAction(tr("蓝色"), [this, circle]() { circle->setColor(Qt::blue); update(); });
        colorMenu->addAction(tr("绿色"), [this, circle]() { circle->setColor(Qt::green); update(); });
        colorMenu->addAction(tr("黑色"), [this, circle]() { circle->setColor(Qt::black); update(); });
        colorMenu->addAction(tr("自定义..."), [this, circle]() {
            QColor color = QColorDialog::getColor(circle->getColor(), this, tr("选择圆线条颜色"));
            if (color.isValid()) {
                circle->setColor(color);
                update();
            }
        });

        QMenu* lineWidthMenu = menu.addMenu(tr("线条宽度"));
        lineWidthMenu->addAction("1px", [this, circle]() { circle->setSize(1.0); update(); });
        lineWidthMenu->addAction("2px", [this, circle]() { circle->setSize(2.0); update(); });
        lineWidthMenu->addAction("3px", [this, circle]() { circle->setSize(3.0); update(); });
        lineWidthMenu->addAction("5px", [this, circle]() { circle->setSize(5.0); update(); });


        menu.addAction(tr("编辑半径..."), [this, circle]() {
            bool ok;
            double currentRadius = circle->getRadius();
            double newRadius = QInputDialog::getDouble(this, tr("设置半径"), tr("半径:"), currentRadius, 0.1, 10000.0, 2, &ok);
            if (ok && newRadius > 0) {
                circle->setRadius(newRadius);
                update();
            }
        });

        menu.addAction(tr("标签..."), [this, circle]() {
            bool ok;
            QString text = QInputDialog::getText(this, tr("设置标签"), tr("标签文本:"), QLineEdit::Normal, circle->getLabel(), &ok);
            if (ok) {
                circle->setLabel(text);
                update();
            }
        });
    }

    // --- 通用菜单项 (适用于所有可选中的对象) ---
    menu.addSeparator(); // 分隔线

    if (contextMenuObj->isHidden()) {
        menu.addAction(tr("显示对象"), [this, contextMenuObj]() {
            contextMenuObj->setHidden(false);
            update();
        });
    } else {
        menu.addAction(tr("隐藏对象"), [this, contextMenuObj]() {
            contextMenuObj->setHidden(true);
            // 如果隐藏了对象，可能需要取消其选中状态
            // contextMenuObj->setSelected(false);
            // selectedObjs_.erase(contextMenuObj);
            update();
        });
    }

    // 删除操作 (作用于所有当前选中的对象)
    if (!selectedObjs_.empty()) {
        menu.addAction(tr("删除选中对象"), [this]() {
            // 创建一个要删除对象的副本，因为在遍历时修改集合可能导致问题
            std::vector<GeometricObject*> toDelete(selectedObjs_.begin(), selectedObjs_.end());
            selectedObjs_.clear(); // 清空选中集合

            for (GeometricObject* obj : toDelete) {
                // 从 objects_ 列表中移除
                objects_.erase(std::remove(objects_.begin(), objects_.end(), obj), objects_.end());
                // 如果 obj 是 hoveredObj_，重置它
                if (hoveredObj_ == obj) {
                    hoveredObj_ = nullptr;
                }
                // TODO: 处理父子关系解除！如果被删除的对象是其他对象的父对象（例如点是圆心），
                // 需要通知子对象。GeometricObject的析构函数或removeParent/Child应该处理。
                delete obj; // 释放对象内存
            }
            update();
        });
    }


    if (!menu.isEmpty()) { // 如果菜单中有任何项，则显示它
        menu.exec(event->globalPos()); // 在鼠标光标的全局位置显示菜单
    }
}


GeometricObject* Canvas::findObjNear(const QPointF& pos) const {
    // 为了让用户更容易选中，可以考虑优先返回选中的对象（如果多个对象重叠）
    // 或者优先返回最上层的对象（如果你的对象有层级/绘制顺序）
    // 当前实现是返回第一个检测到的对象
    for (auto it = objects_.rbegin(); it != objects_.rend(); ++it) { // 从后往前遍历，模拟点击最上层对象
        GeometricObject* obj = *it;
        if (obj && !obj->isHidden() && obj->isNear(pos)) {
            return obj;
        }
    }
    return nullptr;
}

Point* Canvas::findPointNear(const QPointF& pos) const {
    for (auto it = objects_.rbegin(); it != objects_.rend(); ++it) {
        GeometricObject* obj = *it;
        if (obj && !obj->isHidden() && obj->getObjectType() == ObjectType::Point && obj->isNear(pos)) {
            return dynamic_cast<Point*>(obj);
        }
    }
    return nullptr;
}

void Canvas::clearSelections() {
    for (auto* obj : selectedObjs_) { // 只需要遍历已选中的对象
        if(obj) obj->setSelected(false);
    }
    selectedObjs_.clear();
}

