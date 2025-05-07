#include "mainwindow.h"
#include "canvas.h"
#include <QButtonGroup>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    // Create canvas
    canvas = new Canvas(this);
    setCentralWidget(canvas);

    // Create toolbar with buttons
    QToolBar* toolbar = new QToolBar("Modes");
    btnSelect = new QPushButton("Select");
    btnPointCreate = new QPushButton("Point");

    // Make buttons checkable
    btnSelect->setCheckable(true);
    btnPointCreate->setCheckable(true);

    btnSelect->setFixedSize(60, 50);
    btnPointCreate->setFixedSize(60, 50);

    // Group buttons (mutually exclusive)
    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(btnSelect);
    group->addButton(btnPointCreate);

    toolbar->addWidget(btnSelect);
    toolbar->addWidget(btnPointCreate);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // Connect signals
    connect(btnSelect, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::SelectionMode);
       canvas->setOperation(nullptr);
    });
    connect(btnPointCreate, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::CreatePointMode);
    });

    // Set initial mode
    btnSelect->click();
    setWindowTitle("Sketchpad");
    resize(800, 600);
}
