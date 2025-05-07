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
    btnCreate = new QPushButton("Point");

    // Make buttons checkable
    btnSelect->setCheckable(true);
    btnCreate->setCheckable(true);

    btnSelect->setFixedSize(60, 50);
    btnCreate->setFixedSize(60, 50);

    // Group buttons (mutually exclusive)
    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(btnSelect);
    group->addButton(btnCreate);

    toolbar->addWidget(btnSelect);
    toolbar->addWidget(btnCreate);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // Connect signals
    connect(btnSelect, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::SelectionMode);
    });
    connect(btnCreate, &QPushButton::clicked, [this]() {
        canvas->setMode(Canvas::CreatePointMode);
    });

    // Set initial mode
    btnSelect->click();
    setWindowTitle("Sketchpad");
    resize(800, 600);
}
