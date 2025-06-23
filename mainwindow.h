#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canvas.h"

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
    Canvas *m_canvas;

private slots: // 声明槽函数
    void onToolSelected(QAbstractButton *button); // <--- 用于响应工具按钮组的点击

private:
    void setupToolPanel();
    // 将 createToolButton 变为 MainWindow 的成员函数
    QToolButton* createToolButton(const QString& text, const QString& iconPath, const QString& tooltip);

    QWidget *toolPanelContent;
    QScrollArea *scrollArea;

    QButtonGroup *m_toolButtonGroup;
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
