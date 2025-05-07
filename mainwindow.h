#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>

class Canvas; // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QPushButton* btnSelect;
    QPushButton* btnPointCreate;
    Canvas* canvas;
};

#endif // MAINWINDOW_H
