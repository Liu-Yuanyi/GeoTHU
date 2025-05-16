#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 前向声明 (Forward declarations)
class QDockWidget;
class QVBoxLayout;
class QGroupBox;
class QToolButton;
class QScrollArea;
class QButtonGroup;     // <--- 新增
class QAbstractButton;  // <--- 新增
class Canvas;           // <--- 新增 (假设您的Canvas类定义在 "canvas.h")

class MainWindow : public QMainWindow
{
    Q_OBJECT // Qt的宏，用于信号和槽

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots: // 声明槽函数
    void onToolSelected(QAbstractButton *button); // <--- 用于响应工具按钮组的点击

private:
    void setupToolPanel();
    // 将 createToolButton 变为 MainWindow 的成员函数
    QToolButton* createToolButton(const QString& text, const QString& iconPath, const QString& tooltip);

    QWidget *toolPanelContent;
    QScrollArea *scrollArea;

    QButtonGroup *m_toolButtonGroup; // <--- 用于管理工具按钮
    Canvas *m_canvas;                // <--- 指向您的Canvas对象
    // 注意: 您需要在 mainwindow.cpp 中 #include "canvas.h" (您的Canvas头文件)
};

#endif // MAINWINDOW_H
