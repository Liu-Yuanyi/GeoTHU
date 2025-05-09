#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include "threeinonebutton.h"

class Canvas; // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QPushButton* btnSelect;
    QPushButton* btnPointCreate;
    QPushButton* btnCircleCreate;
    ThreeInOneButton* btnLineCreate;
    Canvas* canvas;

    void setupToolbar();
};

#endif // MAINWINDOW_H
