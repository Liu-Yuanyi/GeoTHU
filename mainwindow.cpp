#include "mainwindow.h"
#include "canvas.h"
#include <QButtonGroup> // 用于按钮分组

void MainWindow::setupToolbar() {
    // 创建带按钮的工具栏
    QToolBar* toolbar = new QToolBar("模式"); // 工具栏标题为 "模式"

    // 创建各个独立的按钮
    btnSelect = new QPushButton("选择");      // "Select" 按钮
    btnPointCreate = new QPushButton("点");   // "Point" 按钮
    btnCircleCreate = new QPushButton("圆");  // "Circle" 按钮

    // 创建三合一按钮 (默认为线段)
    btnLineCreate = new ThreeInOneButton(this);
    btnLineCreate->mainButton()->setChecked(true); // 确保它初始状态为选中 (蓝色)

    // 使独立按钮可选中 (checkable)
    btnSelect->setCheckable(true);
    btnPointCreate->setCheckable(true);
    btnCircleCreate->setCheckable(true);

    // 为独立按钮设置固定大小
    btnSelect->setFixedSize(60, 50);
    btnPointCreate->setFixedSize(60, 50);
    btnCircleCreate->setFixedSize(60, 50);

    // 将所有相关按钮分组 (它们将互斥)
    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(btnSelect);
    group->addButton(btnPointCreate);
    group->addButton(btnLineCreate->mainButton());    // 三合一按钮的主按钮
    group->addButton(btnLineCreate->segmentButton()); // 三合一按钮的线段选项
    group->addButton(btnLineCreate->arrayButton());   // 三合一按钮的射线选项 (原文 array, 可能指 Ray)
    group->addButton(btnLineCreate->lineButton());    // 三合一按钮的直线选项
    group->addButton(btnCircleCreate);

    // 将小部件添加到工具栏
    toolbar->addWidget(btnSelect);
    toolbar->addWidget(btnPointCreate);
    toolbar->addWidget(btnLineCreate); // 添加三合一按钮
    toolbar->addWidget(btnCircleCreate);

    // 将工具栏添加到主窗口的左侧工具栏区域
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // 连接现有按钮的信号
    connect(btnSelect, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::SelectionMode); // 设置画布为选择模式
        canvas->setOperation(nullptr);          // 清除当前操作
    });

    connect(btnPointCreate, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::CreatePointMode); // 设置画布为创建点模式
        canvas->setOperation(nullptr);            // 清除当前操作
    });

    // 连接三合一按钮选项的信号
    connect(btnLineCreate, &ThreeInOneButton::segmentClicked, [this]() { // 线段按钮点击
        canvas->setMode(Canvas::OperationMode);     // 设置画布为操作模式 (具体操作由 setOperation 决定)
        // 强制主按钮保持选中状态 (蓝色)
        btnLineCreate->mainButton()->setChecked(true);
        canvas->setOperation(nullptr); // 可能需要设置为具体的线段操作
    });

    connect(btnLineCreate, &ThreeInOneButton::arrayClicked, [this]() { // 射线按钮点击 (原文 array)
        canvas->setMode(Canvas::OperationMode);
        // 强制主按钮保持选中状态 (蓝色)
        btnLineCreate->mainButton()->setChecked(true);
        canvas->setOperation(nullptr); // 可能需要设置为具体的射线操作
    });

    connect(btnLineCreate, &ThreeInOneButton::lineClicked, [this]() { // 直线按钮点击
        canvas->setMode(Canvas::OperationMode);
        // 强制主按钮保持选中状态 (蓝色)
        btnLineCreate->mainButton()->setChecked(true);
        canvas->setOperation(nullptr); // 可能需要设置为具体的直线操作
    });

    // 初始时激活线段模式 (通过模拟点击主按钮)
    btnLineCreate->mainButton()->click();

    connect(btnCircleCreate, &QPushButton::clicked, [this]() {
        // ***** 将画布模式设置为创建圆模式 *****
        canvas->setMode(Canvas::CreateCircleMode); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 修正模式
        canvas->setOperation(nullptr); // 清除任何可能激活的操作是个好习惯
    });
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    // 创建画布
    canvas = new Canvas(this);
    setCentralWidget(canvas); // 将画布设置为主窗口的中央部件

    setupToolbar(); // 设置工具栏

    // 设置初始模式 (例如，默认选中 "选择" 按钮)
    btnSelect->click();
    setWindowTitle("画板"); // 设置窗口标题为 "画板" (Sketchpad)
    resize(800, 600);    // 设置窗口初始大小
}
