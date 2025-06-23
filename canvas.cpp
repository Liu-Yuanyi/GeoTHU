#include "canvas.h"
#include <QPainter>
#include <QInputDialog> // 确保包含 QInputDialog
#include <QColorDialog> // 确保包含 QColorDialog
#include <QFileDialog>
#include <cmath>        // For std::sqrt, std::pow, std::abs (QLineF::length() 也可以)
#include <algorithm>    // For std::remove if deleting objects
#include "geometricobject.h"
#include "circle.h"
#include "line.h"
#include "lineo.h"
#include "lineoo.h"
#include "tools.h"
#include "intersectioncreator.h"
#include "saveloadhelper.h"
#include <stack>

// 假设你的 ObjectType 和 ObjectName 在 "objecttype.h" (或其他地方) 定义，并且 GetDefault... 映射存在
// extern std::map<ObjectType, QColor> GetDefaultColor;
// extern std::map<ObjectType, double> GetDefaultSize;
// extern std::map<ObjectType, QString> GetDefaultLable;

const int maxCacheSize = 200;

std::set<GeometricObject*> showObjectsCache;

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true); // 开启鼠标跟踪以接收 mouseMoveEvent (即使没有按钮按下)
    currentOperation_ = nullptr; // 初始化
    currentMode = SelectionMode;
    setFocusPolicy(Qt::StrongFocus);
    filePath_ = "";
    saved_ = false;
    operations.push_back(new TwoPointCircleCreator());
    operations.push_back(new LineCreator());
    operations.push_back(new LineooCreator());
    operations.push_back(new PerpendicularBisectorCreator());
    operations.push_back(new ParallelLineCreator());
    operations.push_back(new LineoCreator());
    operations.push_back(new MidpointCreator());
    operations.push_back(new PerpendicularLineCreator());
    operations.push_back(new AngleBisectorCreator());
    operations.push_back(new TangentLineCreator());
    operations.push_back(new IntersectionCreator());
    operations.push_back(new CenterRadiusCircleCreator()); // 索引11
    operations.push_back(new ThreePointCircleCreator());   // 索引12
    operations.push_back(new CenterTwoPointArcCreator());               // 索引13
    operations.push_back(new SemicircleCreator());        // 索引14
    operations.push_back(new AxialSymmetry());             //索引15
    operations.push_back(new CentralSymmetry());
    // TODO: add other operations here.

    cacheObj_ = std::vector<std::vector<GeometricObject*>>(maxCacheSize, std::vector<GeometricObject*>());
    cacheDel_ = std::vector<std::vector<GeometricObject*>>(maxCacheSize, std::vector<GeometricObject*>());
    cacheHidden_ = std::vector<std::vector<bool>>(maxCacheSize, std::vector<bool>());
    cachePos_ = std::vector<std::vector<QPointF>>(maxCacheSize, std::vector<QPointF>());
}

Canvas::~Canvas(){
    for (auto obj : objects_){
        delete obj;
    }
    for (auto operation : operations){
        delete operation;
    }
}

void Canvas::setMode(Mode newMode) {
    currentMode = newMode;
    clearSelections(); // 切换模式时是否清除选择，根据需求决定
    update(); // 更新画布显示，例如模式切换可能有视觉提示
}

void Canvas::setOperation(const int index) {
    this->currentOperation_ = operations[index];
}

void Canvas::updateHoverState(const QPointF& pos) {
    std::vector<GeometricObject*> newHover;
    Point* p = findPointNear(pos);
    if (p) {
        p->setHovered(true);
        newHover.push_back(p);
    } else {
        newHover = findObjectsNear(pos);
        for (auto obj : newHover) {
            obj->setHovered(true);
        }
    }
    for (auto obj : hoveredObjs_) {
        if (std::find(newHover.begin(), newHover.end(), obj) == newHover.end()){
            obj->setHovered(false);
        }
    }
    hoveredObjs_ = newHover;
    if (hoveredObjs_.size() == 1 and !tempObjects_.empty()){
        if (hoveredObjs_[0]->getObjectType() == ObjectType::Point) {
            tempObjects_[0]->setHidden(true);
        }
    } else if (!tempObjects_.empty()) {
        tempObjects_[0]->setHidden(false);
    }
    update();
}

GeometricObject* Canvas::automaticIntersection(const QPointF& pos) {
    QString neededlabel=GetDefaultLable[ObjectType::Point];
    std::vector<GeometricObject*> objsNear = findObjectsNear(pos);
    if (objsNear.size() >= 2){
        std::vector<GeometricObject*> v = {objsNear[0], objsNear[1]};
        auto newObjects = operations[10]->apply(v);
        qDebug()<<"newObject.size() = "<<newObjects.size();
        GeometricObject* targetObj = nullptr;
        long double mindist=1e100;
        for(auto iter:newObjects){
            if(!iter->isLegal()){
                delete iter;
                continue;
            }
            if(targetObj==nullptr){
                targetObj=iter;
                mindist=len(pos-iter->position());
                continue;
            }
            if(len(pos-iter->position())<mindist){
                delete targetObj;
                targetObj=iter;
                mindist=len(pos-iter->position());
            }
            else{
                delete iter;
            }
        }
        if(targetObj){
                targetObj->setLabel(neededlabel);
                GetDefaultLable[ObjectType::Point]=nextPointLable(neededlabel);
                objects_.push_back(targetObj);
                loadInCache();
                targetObj->setSelected(true);
                selectedObjs_.insert(targetObj);
            }
        return targetObj;
    }
    return nullptr;
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    mousePos_ = event->position(); // 记录鼠标按下位置，主要用于拖拽计算

    if (event->button() == Qt::LeftButton) {
        initialPositions_.clear(); // 清空，为新的拖拽或点选操作做准备

        if (currentMode == SelectionMode) {
            isDuringMultipleSelection_ = false;
            if (automaticIntersection(mousePos_)){
                return;
            }
            GeometricObject* clickedObj = findObjNear(mousePos_);
            if (clickedObj) {
                if (!clickedObj->isSelected()) {
                    if (!(event->modifiers() & Qt::ControlModifier)) {
                        clearSelections();
                    }
                    clickedObj->setSelected(true);
                    selectedObjs_.insert(clickedObj);
                } else {
                    if (event->modifiers() & Qt::ControlModifier) {
                        clickedObj->setSelected(false);
                        selectedObjs_.erase(clickedObj);
                    }
                }
            } else { // 点击到空白区域
                if (!(event->modifiers() & Qt::ControlModifier)) { // 如果没有按下Ctrl键
                    clearSelections();
                    multipleSelectionStartPos_ = mousePos_;
                }
            }
            // 为所有选中的对象记录初始位置，用于拖动
            for (auto obj : selectedObjs_) {
                initialPositions_[obj] = obj->position();
            }
        } else if (currentMode == CreatePointMode) {
            clearSelections(); // 创建新对象前通常清除选择
            Point* existingPoint = findPointNear(mousePos_);
            if (!existingPoint) { // 如果附近没有点，则创建新点
                if (automaticIntersection(mousePos_)){
                    return;
                }
                std::vector<GeometricObject*> objsNear = findObjectsNear(mousePos_);
                GeometricObject* newPoint;
                if (objsNear.size() == 1){
                    GeometricObject* constraintObj = objsNear[0];
                    if (constraintObj->getObjectType() == ObjectType::Circle) {
                        newPoint = (new Point(objsNear, 4));
                        dynamic_cast<Point*>(newPoint)->setPosition(mousePos_);
                        newPoint->flush();
                    } else {
                        newPoint = (new Point(objsNear, 3));
                        dynamic_cast<Point*>(newPoint)->setPosition(mousePos_);
                        newPoint->flush();
                    }
                } else {
                    newPoint = (new Point(mousePos_))->flush();
                }
                objects_.push_back(newPoint);
                loadInCache();
                newPoint->setSelected(true); // 新创建的点默认为选中状态
                selectedObjs_.insert(newPoint);
            } else { // 如果点击在现有点附近，则选中该点
                existingPoint->setSelected(true);
                selectedObjs_.insert(existingPoint);
            }
        }


        //0 找到完美符合
        //1 确定不可能符合
        //2 有可能符合，且下一个一定是点
        //3 有可能符合，且下一个不可能是点
        //4 有可能符合，且下一个有可能是点，但不一定是点
        else if (currentMode == OperationMode) {
            if (currentOperation_->isValidInput(operationSelections_) == 1){ //不可能符合
                clearSelections();
            } else if (currentOperation_->isValidInput(operationSelections_) == 2){ //下一个一定是点
                GeometricObject* possiblePoint = automaticIntersection(mousePos_);
                GeometricObject* targetPoint;
                std::vector<GeometricObject*> objsNear = findObjectsNear(mousePos_);
                if (possiblePoint) {
                    targetPoint = dynamic_cast<Point*>(possiblePoint);
                } else if (objsNear.size() == 1) {
                    GeometricObject* constraintObj = objsNear[0];
                    if (constraintObj->getObjectType() == ObjectType::Circle) {
                        targetPoint = (new Point(objsNear, 4));
                        dynamic_cast<Point*>(targetPoint)->setPosition(mousePos_);
                        targetPoint->flush();
                    } else {
                        targetPoint = (new Point(objsNear, 3));
                        dynamic_cast<Point*>(targetPoint)->setPosition(mousePos_);
                        targetPoint->flush();
                    }
                    objects_.push_back(targetPoint);
                    loadInCache();
                } else {
                    targetPoint = findPointNear(mousePos_);
                    if (!targetPoint) { // 如果附近没有点，则创建新点
                        GeometricObject* newPoint = (new Point(mousePos_))->flush();
                        objects_.push_back(newPoint);
                        loadInCache();
                        targetPoint = newPoint;
                    }
                }
                targetPoint->setSelected(true);
                selectedObjs_.insert(targetPoint);
                if (std::find(operationSelections_.begin(), operationSelections_.end(), targetPoint) != operationSelections_.end()){
                    clearSelections();
                    operationSelections_.clear();
                    tempObjects_.clear();
                } else {
                    operationSelections_.push_back(targetPoint);
                }
            } else if (currentOperation_->isValidInput(operationSelections_) == 3){
                GeometricObject* targetObj = findObjNear(mousePos_);
                if (!targetObj) {
                    clearSelections();
                    operationSelections_.clear();
                    tempObjects_.clear();
                }
                else{
                    targetObj->setSelected(true);
                    selectedObjs_.insert(targetObj);
                    if (std::find(operationSelections_.begin(), operationSelections_.end(), targetObj) != operationSelections_.end()){
                        clearSelections();
                        operationSelections_.clear();
                        tempObjects_.clear();
                    } else {
                        operationSelections_.push_back(targetObj);
                    }
                    if (currentOperation_->isValidInput(operationSelections_) == 1){
                        clearSelections();
                        operationSelections_.clear();
                        tempObjects_.clear();
                    }
                }
            } else if (currentOperation_->isValidInput(operationSelections_) == 4){
                Point* targetPoint = findPointNear(mousePos_);
                if (targetPoint){
                    targetPoint->setSelected(true);
                    selectedObjs_.insert(targetPoint);
                    if (std::find(operationSelections_.begin(), operationSelections_.end(), targetPoint) != operationSelections_.end()){
                        clearSelections();
                        operationSelections_.clear();
                        tempObjects_.clear();
                    } else {
                        operationSelections_.push_back(targetPoint);
                    }
                }
                else{
                    GeometricObject* targetObj = findObjNear(mousePos_);
                    if (targetObj){
                        targetObj->setSelected(true);
                        selectedObjs_.insert(targetObj);
                        if (std::find(operationSelections_.begin(), operationSelections_.end(), targetObj) != operationSelections_.end()){
                            clearSelections();
                            operationSelections_.clear();
                            tempObjects_.clear();
                        } else {
                            operationSelections_.push_back(targetObj);
                        }
                    }
                    else{
                        GeometricObject* newPoint = (new Point(mousePos_))->flush();
                        objects_.push_back(newPoint);
                        loadInCache();
                        selectedObjs_.insert(newPoint);
                        if (std::find(operationSelections_.begin(), operationSelections_.end(), newPoint) != operationSelections_.end()){
                            clearSelections();
                            operationSelections_.clear();
                            tempObjects_.clear();
                        } else {
                            operationSelections_.push_back(newPoint);
                        }
                    }
                }
            }
            if (currentOperation_->isValidInput(operationSelections_) == 0){ //已经符合
                std::set<GeometricObject*> newObject = currentOperation_->apply(operationSelections_);
                clearSelections();
                clearTempObjects();
                for (auto obj : newObject){
                    obj->setSelected(true);
                    objects_.push_back(obj);
                    selectedObjs_.insert(obj);
                }
                loadInCache();
            } else if (currentOperation_->isWaiting(operationSelections_) and currentOperation_->waitImplemented){
                if (tempObjects_.empty()) {
                    Point* p = new Point(mousePos_, true);
                    p->setSelected(false);
                    p->setHidden(false);
                    operationSelections_.push_back(p);
                    tempObjects_.push_back(p);
                    std::set<GeometricObject*> temps = currentOperation_->wait(operationSelections_);
                    for (auto obj : temps){
                        obj->setSelected(false);
                        obj->setHidden(false);
                        tempObjects_.push_back(obj);
                    }
                    operationSelections_.erase(operationSelections_.end() - 1);
                    qDebug() << "size: " << operationSelections_.size();
                }
            }
        }
        update();
    }



    else if (event->button() == Qt::RightButton) { // 右键点击
        clearSelections();
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
        update();
        // Qt 会自动在之后调用 contextMenuEvent
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentPos = event->position();
    updateHoverState(currentPos); // 实时更新悬停对象

    if (currentMode == SelectionMode) {
        if (!selectedObjs_.empty() && (event->buttons() & Qt::LeftButton) && !isDuringMultipleSelection_) { // 如果有选中的对象并且按住左键拖动
            QPointF delta = currentPos - mousePos_; // 计算拖动向量
            for (auto obj : selectedObjs_) {
                QPointF newPos = initialPositions_[obj] + delta; // 计算新位置
                if (obj->getObjectType() == ObjectType::Point) {
                    Point* point = dynamic_cast<Point*>(obj);
                    if (point) point->setPosition(newPos);
                }
            }
            if (len(delta) > 0) {
                hasMoved_ = true;
            }
            update();
        } else if ((event->buttons() & Qt::LeftButton) && isDuringMultipleSelection_) {
            multipleSelectionEndPos_ = currentPos;
            for (auto obj : objects_){
                if (obj->isShown()){
                    if (obj->isTouchedByRectangle(multipleSelectionStartPos_, multipleSelectionEndPos_)){
                        obj->setSelected(true);
                        selectedObjs_.insert(obj);
                    } else {
                        obj->setSelected(false);
                        auto iter = selectedObjs_.find(obj);
                        if (iter != selectedObjs_.end()) {
                            selectedObjs_.erase(iter);
                        }
                    }
                }
            }
        } else if (event->buttons() & Qt::LeftButton) {
            multipleSelectionEndPos_ = currentPos;
            isDuringMultipleSelection_ = true;
        }
    } else if (currentMode == OperationMode) {
        if (!tempObjects_.empty() and currentOperation_->waitImplemented) {
            Point* p = dynamic_cast<Point*>(tempObjects_[0]);
            p->setPosition(currentPos);
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    QPointF releasePos = event->position();
    if (event->button() == Qt::LeftButton) {
        if (currentMode == SelectionMode) {
            isDuringMultipleSelection_ = false;
            if (hasMoved_) { // 如果之前是拖拽或按下时选中
                loadInCache();
                hasMoved_ = false;
            }
        } else if (currentMode == OperationMode) {
            if (currentOperation_->waitImplemented and !tempObjects_.empty() and
                len(releasePos - mousePos_) > 10) {
                clearTempObjects();

                GeometricObject* possiblePoint = automaticIntersection(releasePos);
                GeometricObject* targetPoint;
                std::vector<GeometricObject*> objsNear = findObjectsNear(releasePos);
                if (possiblePoint) {
                    targetPoint = dynamic_cast<Point*>(possiblePoint);
                } else if (objsNear.size() == 1) {
                    GeometricObject* constraintObj = objsNear[0];
                    if (constraintObj->getObjectType() == ObjectType::Circle) {
                        targetPoint = (new Point(objsNear, 4));
                        dynamic_cast<Point*>(targetPoint)->setPosition(releasePos);
                        targetPoint->flush();
                    } else {
                        targetPoint = (new Point(objsNear, 3));
                        dynamic_cast<Point*>(targetPoint)->setPosition(releasePos);
                        targetPoint->flush();
                    }
                    objects_.push_back(targetPoint);
                    loadInCache();
                } else {
                    targetPoint = findPointNear(releasePos);
                    if (!targetPoint) { // 如果附近没有点，则创建新点
                        GeometricObject* newPoint = (new Point(releasePos))->flush();
                        objects_.push_back(newPoint);
                        loadInCache();
                        targetPoint = newPoint;
                    }
                }
                targetPoint->setSelected(false);
                if (std::find(operationSelections_.begin(), operationSelections_.end(), targetPoint) != operationSelections_.end()){
                    clearSelections();
                    operationSelections_.clear();
                    tempObjects_.clear();
                } else {
                    operationSelections_.push_back(targetPoint);
                    std::set<GeometricObject*> newObject = currentOperation_->apply(operationSelections_);
                    clearSelections();
                    for (auto obj : newObject){
                        obj->setSelected(true);
                        objects_.push_back(obj);
                        selectedObjs_.insert(obj);
                    }
                    loadInCache();
                }
            }
        }
    }
    update();
}

void Canvas::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿，使图形更平滑

    if (isDuringMultipleSelection_) {
        QPen pen(Qt::black);             // solid black edge
        pen.setWidth(2);                 // thickness of the edge
        painter.setPen(pen);

        // Set light fill color (brush)
        QBrush brush(Qt::lightGray);     // light gray fill
        painter.setBrush(brush);

        long double x = multipleSelectionStartPos_.x(), y = multipleSelectionStartPos_.y();
        long double dx = multipleSelectionEndPos_.x() - x;
        long double dy = multipleSelectionEndPos_.y() - y;
        QRect rect(x, y, dx, dy);    // x, y, width, height
        painter.drawRect(rect);

        painter.setBrush(Qt::NoBrush);
    }

    if (!showObjectsCache.empty()){
        for (auto obj : showObjectsCache){
            obj->setSelected(true);
            selectedObjs_.insert(obj);
        }
        showObjectsCache.clear();
    }

    // 绘制所有正式的几何对象
    for (auto* obj : objects_) {
        obj->flush();
    }
    for (auto* obj : tempObjects_) {
        obj->flush();
    }
    for (const auto* obj : objects_) {
        if (obj->isShown() and obj->getObjectType() != ObjectType::Point){
            obj->draw(&painter);
        }
    }
    for (const auto* obj : tempObjects_) {
        if (obj->isShown() and obj->getObjectType() != ObjectType::Point){
            obj->draw(&painter);
        }
    }
    for (const auto* obj : objects_) {
        if (obj->isShown() and obj->getObjectType() == ObjectType::Point){
            obj->draw(&painter);
        }
    }
    for (const auto* obj : tempObjects_) {
        if (obj->isShown() and obj->getObjectType() == ObjectType::Point){
            obj->draw(&painter);
        }
    }
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

        QMenu* colorMenu = menu.addMenu(tr("color")); // tr() 用于国际化
        colorMenu->addAction(tr("red"), [this, point]() { point->setColor(Qt::red); update(); });
        colorMenu->addAction(tr("blue"), [this, point]() { point->setColor(Qt::darkBlue); update(); });
        colorMenu->addAction(tr("green"), [this, point]() { point->setColor(Qt::darkGreen); update(); });
        colorMenu->addAction(tr("black"), [this, point]() { point->setColor(Qt::black); update(); });
        colorMenu->addAction(tr("customize..."), [this, point]() {
            QColor color = QColorDialog::getColor(point->getColor(), this, tr("select color"));
            if (color.isValid()) {
                point->setColor(color);
                update();
            }
        });

        QMenu* sizeMenu = menu.addMenu(tr("size"));
        sizeMenu->addAction(tr("tiny"), [this, point]() { point->setSize(2); update(); });
        sizeMenu->addAction(tr("small"),   [this, point]() { point->setSize(3); update(); });
        sizeMenu->addAction(tr("medium"),   [this, point]() { point->setSize(4); update(); });
        sizeMenu->addAction(tr("large"),   [this, point]() { point->setSize(5); update(); });

        menu.addAction(tr("label..."), [this, point]() {
            bool ok;
            QString text = QInputDialog::getText(this, tr("set label"), tr("label:"), QLineEdit::Normal, point->getLabel(), &ok);
            if (ok) {
                point->setLabel(text);
                update();
            }
        });

    } else if (contextMenuObj->getObjectType() == ObjectType::Line or
               contextMenuObj->getObjectType() == ObjectType::Lineo or
               contextMenuObj->getObjectType() == ObjectType::Lineoo){

        QMenu* colorMenu = menu.addMenu(tr("color")); // tr() 用于国际化
        colorMenu->addAction(tr("red"), [this, contextMenuObj]() { contextMenuObj->setColor(Qt::red); update(); });
        colorMenu->addAction(tr("blue"), [this, contextMenuObj]() { contextMenuObj->setColor(Qt::darkBlue); update(); });
        colorMenu->addAction(tr("green"), [this, contextMenuObj]() { contextMenuObj->setColor(Qt::darkGreen); update(); });
        colorMenu->addAction(tr("black"), [this, contextMenuObj]() { contextMenuObj->setColor(Qt::black); update(); });
        colorMenu->addAction(tr("customize..."), [this, contextMenuObj]() {
            QColor color = QColorDialog::getColor(contextMenuObj->getColor(), this, tr("select color"));
            if (color.isValid()) {
                contextMenuObj->setColor(color);
                update();
            }
        });

        QMenu* sizeMenu = menu.addMenu(tr("thickness"));
        sizeMenu->addAction(tr("thin"), [this, contextMenuObj]() { contextMenuObj->setSize(1); update(); });
        sizeMenu->addAction(tr("medium"),   [this, contextMenuObj]() { contextMenuObj->setSize(2); update(); });
        sizeMenu->addAction(tr("thick"),   [this, contextMenuObj]() { contextMenuObj->setSize(3); update(); });
        sizeMenu->addAction(tr("ultra-thick"),   [this, contextMenuObj]() { contextMenuObj->setSize(4); update(); });

        QMenu* shapeMenu = menu.addMenu(tr("linestyle"));
        shapeMenu->addAction(tr("solid"), [this, contextMenuObj]() { contextMenuObj->setShape(0); update(); });
        shapeMenu->addAction(tr("dashed"),   [this, contextMenuObj]() { contextMenuObj->setShape(1); update(); });
        shapeMenu->addAction(tr("dotted"),   [this, contextMenuObj]() { contextMenuObj->setShape(2); update(); });

        menu.addAction(tr("label..."), [this, contextMenuObj]() {
            bool ok;
            QString text = QInputDialog::getText(this, tr("set label"), tr("label:"), QLineEdit::Normal, contextMenuObj->getLabel(), &ok);
            if (ok) {
                contextMenuObj->setLabel(text);
                update();
            }
        });
    }
    else if (contextMenuObj->getObjectType() == ObjectType::Circle) { // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增：圆的上下文菜单
        Circle* circle = dynamic_cast<Circle*>(contextMenuObj);
        if (!circle) return;

        QMenu* colorMenu = menu.addMenu(tr("color"));
        colorMenu->addAction(tr("red"), [this, circle]() { circle->setColor(Qt::red); update(); });
        colorMenu->addAction(tr("blue"), [this, circle]() { circle->setColor(Qt::darkBlue); update(); });
        colorMenu->addAction(tr("green"), [this, circle]() { circle->setColor(Qt::darkGreen); update(); });
        colorMenu->addAction(tr("black"), [this, circle]() { circle->setColor(Qt::black); update(); });
        colorMenu->addAction(tr("customize..."), [this, circle]() {
            QColor color = QColorDialog::getColor(circle->getColor(), this, tr("select color"));
            if (color.isValid()) {
                circle->setColor(color);
                update();
            }
        });

        QMenu* lineWidthMenu = menu.addMenu(tr("thickness"));
        lineWidthMenu->addAction("thin", [this, circle]() { circle->setSize(1.0); update(); });
        lineWidthMenu->addAction("medium", [this, circle]() { circle->setSize(2.0); update(); });
        lineWidthMenu->addAction("thick", [this, circle]() { circle->setSize(3.0); update(); });
        lineWidthMenu->addAction("ultra-thick", [this, circle]() { circle->setSize(4.0); update(); });

        QMenu* shapeMenu = menu.addMenu(tr("linestyle"));
        shapeMenu->addAction(tr("solid"), [this, contextMenuObj]() { contextMenuObj->setShape(0); update(); });
        shapeMenu->addAction(tr("dashed"),   [this, contextMenuObj]() { contextMenuObj->setShape(1); update(); });
        shapeMenu->addAction(tr("dotted"),   [this, contextMenuObj]() { contextMenuObj->setShape(2); update(); });

        menu.addAction(tr("label..."), [this, circle]() {
            bool ok;
            QString text = QInputDialog::getText(this, tr("set label"), tr("label:"), QLineEdit::Normal, circle->getLabel(), &ok);
            if (ok) {
                circle->setLabel(text);
                update();
            }
        });
    }

    // --- 通用菜单项 (适用于所有可选中的对象) ---
    menu.addSeparator(); // 分隔线

    menu.addAction(tr("hide"), [this, contextMenuObj]() {
        contextMenuObj->setHidden(true);
        contextMenuObj->setSelected(false);
        loadInCache();
        selectedObjs_.erase(contextMenuObj);
        update();
    });

    menu.addAction(tr(contextMenuObj->islablehidden()?"show label":"hide label"), [this, contextMenuObj]() {
        contextMenuObj->setlabelhidden(1-contextMenuObj->islablehidden());
        contextMenuObj->setSelected(false);
        selectedObjs_.erase(contextMenuObj);
        update();
    });

    // 删除操作 (作用于所有当前选中的对象)

    menu.addAction(tr("delete"), [this]() {
        deleteObjects();
    });



    if (!menu.isEmpty()) { // 如果菜单中有任何项，则显示它
        menu.exec(event->globalPos()); // 在鼠标光标的全局位置显示菜单
    }
}

void Canvas::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up or event->key() == Qt::Key_Down or
        event->key() == Qt::Key_Right or event->key() == Qt::Key_Left) {
        QPointF delta;
        switch (event->key()) {
        case (Qt::Key_Up):
            delta = QPointF(0, 3);
            break;
        case (Qt::Key_Down):
            delta = QPointF(0, -3);
        case (Qt::Key_Right):
            delta = QPointF(-3, 0);
        case (Qt::Key_Left):
            delta = QPointF(3, 0);
        }
        for (auto obj : objects_) {
            if (obj->getObjectType() == ObjectType::Point and obj->getParents().empty()) {
                auto curPosition = obj->position();
                Point* point = dynamic_cast<Point*>(obj);
                point->setPosition(curPosition + delta);
            }
        }
        for (auto& v : cachePos_) {
            for (QPointF& p : v) {
                p += delta;
            }
        }
        update();
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_H) {
        hideObjects();
        update();
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S) {
        saveFile();
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_L) {
        loadFile();
        update();
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z) {
        undo();
        update();
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Y) {
        redo();
        update();
    }
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_A) {
        selectedObjs_.clear();
        for (auto obj : objects_) { // 只需要遍历已选中的对象
            if (obj->isShown()) {
                obj->setSelected(true);
                selectedObjs_.insert(obj);
            }
        }
        update();
    }
    if (event->key() == Qt::Key_Delete) {
        deleteObjects();
        update();
    }
}

void Canvas::wheelEvent(QWheelEvent *event) {
    mousePos_ = event->position();
    if (!(event->modifiers() & Qt::ControlModifier)){
        long double deltay = event->angleDelta().y();
        long double deltax = event->angleDelta().x();
        for (auto obj : objects_) {
            if (obj->getObjectType() == ObjectType::Point and obj->getParents().empty()) {
                auto curPosition = obj->position();
                Point* point = dynamic_cast<Point*>(obj);
                point->setPosition(curPosition + QPointF(0.3 * deltax, 0.3 * deltay));
                update();
            }
        }
        for (auto obj : tempObjects_) {
            if (obj->getObjectType() == ObjectType::Point and obj->getParents().empty()) {
                auto curPosition = obj->position();
                Point* point = dynamic_cast<Point*>(obj);
                point->setPosition(curPosition + QPointF(0.3 * deltax, 0.3 * deltay));
                update();
            }
        }
        for (auto& v : cachePos_) {
            for (QPointF& p : v) {
                p += QPointF(0.3 * deltax, 0.3 * deltay);
            }
        }
    } else {
        long double deltay = event->angleDelta().y();
        for (auto obj : objects_) {
            if (obj->getObjectType() == ObjectType::Point and obj->getParents().empty()) {
                auto curPosition = obj->position();
                Point* point = dynamic_cast<Point*>(obj);
                QPointF center = mousePos_;
                point->setPosition(curPosition * (1 + deltay / 2000) - center * (deltay / 2000));
            }
        }
        for (auto obj : tempObjects_) {
            if (obj->getObjectType() == ObjectType::Point and obj->getParents().empty()) {
                auto curPosition = obj->position();
                Point* point = dynamic_cast<Point*>(obj);
                QPointF center = mousePos_;
                point->setPosition(curPosition * (1 + deltay / 2000) - center * (deltay / 2000));
            }
        }
        for (auto& v : cachePos_) {
            for (QPointF& p : v) {
                p = p * (1 + deltay / 2000) - mousePos_ * (deltay / 2000);
            }
        }
    }
    update();
    event->accept();
}

void Canvas::setFilePath(QString path) {
    filePath_ = path;
}

bool Canvas::saveFile() {
    if (filePath_.isEmpty()) {
        QString path = QFileDialog::getSaveFileName(this, "save", "", "My Files (*.thu)");
        if (path.isEmpty()){
            return false;
        }
        filePath_ = path;
    }

    QFile file(filePath_);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Could not open file:\n" + file.errorString());
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    Saveloadhelper helper;
    out << int(objects_.size());
    qDebug() << objects_.size();
    for (auto obj : objects_) {
        helper.save(obj, out);
    }
    file.close();
    saved_ = true;
    return true;
}

void Canvas::loadFile(bool onStartup) {
    clearObjects();
    clearTempObjects();
    if (!onStartup) {
        QString path = QFileDialog::getOpenFileName(this, "Open", "", "My Files (*.thu)");
        if (path.isEmpty()) {
            return;
        }
        filePath_ = path;
    }

    QFile file(filePath_);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Could not open the file.");
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    int n;
    in >> n;
    qDebug() << n;
    Saveloadhelper helper;
    for (int i = 0; i < n; ++i){
        GeometricObject* obj = helper.load(in);
        objects_.push_back(obj);
    }
    file.close();
    saved_ = true;
}

void Canvas::loadInCache() {
    maxUndoCount_ = std::min(maxCacheSize, maxUndoCount_ + 1);
    maxRedoCount_ = 0;
    currentCacheIndex_ = (currentCacheIndex_ + 1) % maxCacheSize;
    cacheObj_[currentCacheIndex_] = objects_;
    cacheDel_[currentCacheIndex_] = deletedObjs_;
    std::vector<bool> hiddenStates = {};
    std::vector<QPointF> pos = {};
    for (auto obj : objects_) {
        hiddenStates.push_back(obj->isHidden());
        pos.push_back(obj->position());
    }
    cacheHidden_[currentCacheIndex_] = hiddenStates;
    cachePos_[currentCacheIndex_] = pos;
    saved_ = false;
}

void Canvas::undo() {
    clearSelections();
    clearTempObjects();
    operationSelections_.clear();
    if (maxUndoCount_ == 0) {
        return;
    }
    --maxUndoCount_;
    ++maxRedoCount_;
    currentCacheIndex_ = (currentCacheIndex_ + maxCacheSize - 1) % maxCacheSize;
    objects_ = cacheObj_[currentCacheIndex_];
    deletedObjs_ = cacheDel_[currentCacheIndex_];
    for (int i = 0; i < objects_.size(); ++i) {
        objects_[i]->setHidden(cacheHidden_[currentCacheIndex_][i]);
        objects_[i]->setSelected(false);
        if (objects_[i]->getObjectType() == ObjectType::Point) {
            Point* p = dynamic_cast<Point*>(objects_[i]);
            p->setPosition(cachePos_[currentCacheIndex_][i]);
        }
    }
    selectedObjs_.clear();
}

void Canvas::redo() {
    clearSelections();
    clearTempObjects();
    operationSelections_.clear();
    if (maxRedoCount_ == 0) {
        return;
    }
    --maxRedoCount_;
    ++maxUndoCount_;
    currentCacheIndex_ = (currentCacheIndex_ + 1) % maxCacheSize;
    objects_ = cacheObj_[currentCacheIndex_];
    deletedObjs_ = cacheDel_[currentCacheIndex_];
    for (int i = 0; i < objects_.size(); ++i) {
        objects_[i]->setHidden(cacheHidden_[currentCacheIndex_][i]);
        objects_[i]->setSelected(false);
        if (objects_[i]->getObjectType() == ObjectType::Point) {
            Point* p = dynamic_cast<Point*>(objects_[i]);
            p->setPosition(cachePos_[currentCacheIndex_][i]);
        }
    }
    selectedObjs_.clear();
}

void Canvas::deleteObjects(){
    std::vector<GeometricObject*> toDelete(selectedObjs_.begin(), selectedObjs_.end());
    for (auto obj : selectedObjs_) {
        obj->setSelected(false);
    }
    selectedObjs_.clear(); // 清空选中集合
    if (!toDelete.empty()) {
        for (GeometricObject* obj : toDelete) {
            // 从 objects_ 列表中移除
            std::stack<GeometricObject*> s;
            s.push(obj);
            while (!s.empty()){
                GeometricObject* curObj = s.top();
                s.pop();
                auto iter = std::find(objects_.begin(), objects_.end(), curObj);
                if (iter != objects_.end()){
                    objects_.erase(iter);
                    auto children = curObj->getChildren();
                    for (auto child : children){
                        s.push(child);
                    }
                    deletedObjs_.push_back(curObj);
                }
            }
        }
        loadInCache();
    }
    update();
}

void Canvas::hideObjects(){
    if (!selectedObjs_.empty()){
        for (auto obj : selectedObjs_){
            obj->setHidden(true);
        }
        loadInCache();
        selectedObjs_.clear();
    }
}

void Canvas::showObjects(){
    clearSelections();
    showObjectsCache.clear();
    bool flag = false;
    for (auto obj : objects_){
        if (obj->isHidden()) {
            flag = true;
            obj->setHidden(false);
            showObjectsCache.insert(obj);
        }
    }
    if (flag) {
        loadInCache();
    }
}

void Canvas::clearObjects(){
    for (auto obj : objects_){
        delete obj;
    }
    for (auto obj : deletedObjs_){
        delete obj;
    }
    objects_.clear();
    hoveredObjs_.clear();
    selectedObjs_.clear();
    initialPositions_.clear();
    operationSelections_.clear();
    cacheObj_ = std::vector<std::vector<GeometricObject*>>(maxCacheSize, std::vector<GeometricObject*>());
    cacheDel_ = std::vector<std::vector<GeometricObject*>>(maxCacheSize, std::vector<GeometricObject*>());
    cacheHidden_ = std::vector<std::vector<bool>>(maxCacheSize, std::vector<bool>());
    cachePos_ = std::vector<std::vector<QPointF>>(maxCacheSize, std::vector<QPointF>());
    GetDefaultLable={
        {ObjectType::Point, "A"},       // 点的默认标签
        {ObjectType::Line, "line_1"},
        {ObjectType::Lineo, "ray_1"},
        {ObjectType::Lineoo, "segment_1"},
        {ObjectType::Circle, "Circle1"}
        };
    GeometricObject::setCounter(0);
}


GeometricObject* Canvas::findObjNear(const QPointF& pos) const {
    // 为了让用户更容易选中，可以考虑优先返回选中的对象（如果多个对象重叠）
    // 或者优先返回最上层的对象（如果你的对象有层级/绘制顺序）
    // 当前实现是返回第一个检测到的对象

    std::vector<GeometricObject*> v = {};
    for (auto it = objects_.rbegin(); it != objects_.rend(); ++it) { // 从后往前遍历，模拟点击最上层对象
        GeometricObject* obj = *it;
        if (obj && !obj->isHidden() && obj->isNear(pos)) {
            v.push_back(obj);
        }
    }
    for (auto obj : v){
        if (obj->getObjectType() == ObjectType::Point){
            return obj;
        }
    }
    if (!v.empty()){
        return v[0];
    }
    return nullptr;
}

std::vector<GeometricObject*> Canvas::findObjectsNear(const QPointF& pos) const {
    std::vector<GeometricObject*> v = {};
    for (auto it = objects_.rbegin(); it != objects_.rend(); ++it) { // 从后往前遍历，模拟点击最上层对象
        GeometricObject* obj = *it;
        if (obj && !obj->isHidden() && obj->isNear(pos)) {
            v.push_back(obj);
        }
    }
    for (auto obj : v){
        if (obj->getObjectType() == ObjectType::Point){
            return std::vector<GeometricObject*>{};
        }
    }
    return v;
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
    operationSelections_.clear();
}

void Canvas::clearTempObjects(){
    for (auto obj : tempObjects_) {
        if (obj) {
            delete obj;
        }
    }
    tempObjects_.clear();
}
