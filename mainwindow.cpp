#include "mainwindow.h"
#include "canvas.h"
#include <QButtonGroup>

void MainWindow::setupToolbar() {
    // Create toolbar with buttons
    QToolBar* toolbar = new QToolBar("Modes");

    // Create individual buttons
    btnSelect = new QPushButton("Select");
    btnPointCreate = new QPushButton("Point");
    btnCircleCreate = new QPushButton("Circle");

    // Create the three-in-one button (defaults to Segment)
    btnLineCreate = new ThreeInOneButton(this);
    btnLineCreate->mainButton()->setChecked(true); // Ensure it starts checked (blue)

    // Make individual buttons checkable
    btnSelect->setCheckable(true);
    btnPointCreate->setCheckable(true);
    btnCircleCreate->setCheckable(true);

    // Set fixed size for individual buttons
    btnSelect->setFixedSize(60, 50);
    btnPointCreate->setFixedSize(60, 50);
    btnCircleCreate->setFixedSize(60, 50);

    // Group all buttons (they will be mutually exclusive)
    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(btnSelect);
    group->addButton(btnPointCreate);
    group->addButton(btnLineCreate->mainButton());
    group->addButton(btnLineCreate->segmentButton());
    group->addButton(btnLineCreate->arrayButton());
    group->addButton(btnLineCreate->lineButton());
    group->addButton(btnCircleCreate);

    // Add widgets to toolbar
    toolbar->addWidget(btnSelect);
    toolbar->addWidget(btnPointCreate);
    toolbar->addWidget(btnLineCreate);
    toolbar->addWidget(btnCircleCreate);


    // Add toolbar to main window
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // Connect signals for existing buttons
    connect(btnSelect, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::SelectionMode);
        canvas->setOperation(nullptr);
    });

    connect(btnPointCreate, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::CreatePointMode);
        canvas->setOperation(nullptr);
    });

    // Connect signals for the three-in-one button options
    connect(btnLineCreate, &ThreeInOneButton::segmentClicked, [this]() {
        canvas->setMode(Canvas::OperationMode);
        // Force the main button to be checked (blue)
        btnLineCreate->mainButton()->setChecked(true);
        canvas->setOperation(nullptr);
    });

    connect(btnLineCreate, &ThreeInOneButton::arrayClicked, [this]() {
        canvas->setMode(Canvas::OperationMode);
        // Force the main button to be checked (blue)
        btnLineCreate->mainButton()->setChecked(true);
        canvas->setOperation(nullptr);
    });

    connect(btnLineCreate, &ThreeInOneButton::lineClicked, [this]() {
        canvas->setMode(Canvas::OperationMode);
        // Force the main button to be checked (blue)
        btnLineCreate->mainButton()->setChecked(true);
        canvas->setOperation(nullptr);
    });

    // Start with segment mode active
    btnLineCreate->mainButton()->click();

    connect(btnCircleCreate, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::OperationMode);
        canvas->setOperation(nullptr);
    });
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    // Create canvas
    canvas = new Canvas(this);
    setCentralWidget(canvas);

    setupToolbar();
    
    // Set initial mode
    btnSelect->click();
    setWindowTitle("Sketchpad");
    resize(800, 600);
}
