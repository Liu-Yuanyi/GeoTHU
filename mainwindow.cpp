#include "mainwindow.h"
#include "canvas.h"
#include <QWidget>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolButton>
#include <QIcon>
#include <QApplication>
#include <QStyle>
#include <QScrollArea>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    toolPanelContent(nullptr), // 初始化所有指针成员
    scrollArea(nullptr),
    m_toolButtonGroup(nullptr),
    m_canvas(nullptr)
{
    // 1. 创建 Canvas 实例
    m_canvas = new Canvas(this); // 父对象是 MainWindow

    // 2. 将 Canvas 设置为主窗口的中央部件
    setCentralWidget(m_canvas); // <--- 这是关键！

    // 3. 创建工具按钮组
    m_toolButtonGroup = new QButtonGroup(this);
    m_toolButtonGroup->setExclusive(true); // 确保一次只有一个按钮被选中

    // 4. 连接信号：当按钮组中的按钮被点击时，调用 onToolSelected
    connect(m_toolButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, &MainWindow::onToolSelected);

    // 5. 设置工具面板 (setupToolPanel 内部会创建 toolPanelContent)
    setupToolPanel(); // 调用此函数来填充 toolPanelContent

    // 6. 创建滚动区域并将工具面板内容放入其中
    scrollArea = new QScrollArea(this); // 父对象是 MainWindow
    scrollArea->setWidget(toolPanelContent); // 将 toolPanelContent 设置为滚动区域的部件
    scrollArea->setWidgetResizable(true); // 允许滚动区域调整其部件的大小
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 7. 创建 DockWidget 并将滚动区域（包含工具面板）放入其中
    QDockWidget *dockWidget = new QDockWidget(tr("Tools"), this);
    dockWidget->setWidget(scrollArea); // 将 scrollArea 设置为 dock widget 的内容
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget); // 将 dock widget 添加到主窗口

    // 8. 设置样式表
    this->setStyleSheet(R"(
        QDockWidget {
            /* titlebar-close-icon: url(:/icons/close.png); */
            /* titlebar-normal-icon: url(:/icons/undock.png); */
        }

        QDockWidget::title {
            text-align: left;
            background: #e0e0e0;
            padding-left: 5px;
            padding-top: 3px;
            padding-bottom: 3px;
            border: 1px solid #c0c0c0;
        }

        QWidget#toolPanelContentWidget {
            background-color: #f7f7f7;
        }

        QGroupBox {
            background-color: #f0f0f0;
            border: 1px solid #dcdcdc;
            border-radius: 6px;
            margin-top: 2ex; /* 调整 margin-top 使标题有空间 */
            padding: 8px; /* 内边距 */
            font-weight: bold;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left; /* 确保标题在左上角 */
            padding: 0 5px 0 5px; /* 标题的内边距 */
            left: 10px; /* 从边框的左边距调整标题位置 */
            color: #555;
        }

        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 4px;
            padding: 4px;
            margin: 1px;
            color: #333;
            text-align: left;
        }

        QToolButton:hover {
            background-color: #e9e9e9;
            border-color: #c0c0c0;
        }

        QToolButton:pressed {
            background-color: #dcdcdc;
        }

        QToolButton:checked {
            background-color: #d0e0ff;
            border-color: #a0c0ff;
            color: #000;
        }
    )");

    // 9. 设置窗口的初始大小和标题
    resize(1024, 768);
    setWindowTitle(tr("绘图应用"));

    // 10. 默认选中第一个工具 (如果存在) 并设置初始模式
    if (!m_toolButtonGroup->buttons().isEmpty()) {
        QAbstractButton* firstButton = m_toolButtonGroup->buttons().first();
        if (firstButton) {
            firstButton->setChecked(true); // 选中按钮
            onToolSelected(firstButton);   // 调用槽函数以应用初始模式
        }
    }
}

// MainWindow 的析构函数
MainWindow::~MainWindow()
{
    // Qt的父子对象机制会自动处理 m_canvas, m_toolButtonGroup,
    // toolPanelContent (如果它有父对象), scrollArea, 和 dockWidget 的删除。
    // 因此，这里通常不需要显式 delete 它们。
}

// 创建工具按钮的成员函数 (保持不变)
QToolButton* MainWindow::createToolButton(const QString& text, const QString& iconPath, const QString& tooltip) {
    QToolButton *button = new QToolButton(toolPanelContent); // 父对象设为 toolPanelContent
    if (!iconPath.isEmpty()) {
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(24, 24)); // 可以根据需要调整图标大小
    }
    button->setText(text);
    button->setToolTip(tooltip);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); // 图标和文本并排
    button->setCheckable(true); // 使按钮可以被选中

    // 将原始文本（或一个唯一的ID）作为属性存储，以便在槽函数中可靠地识别按钮
    // 使用 text 作为标识符，如果支持多语言，确保 tr(text) 在 onToolSelected 中也使用
    button->setProperty("toolIdentifier", text);

    if (button->icon().isNull() && !text.isEmpty()) {
        button->setToolButtonStyle(Qt::ToolButtonTextOnly); // 如果没有图标，仅显示文本
    }

    m_toolButtonGroup->addButton(button); // 将按钮添加到按钮组
    return button;
}

// 设置工具面板的函数 (保持不变，但注意 createToolButton 的父对象)
void MainWindow::setupToolPanel()
{
    toolPanelContent = new QWidget(); // 创建工具面板的内容 widget
    toolPanelContent->setObjectName("toolPanelContentWidget"); // 用于样式表选择器

    QVBoxLayout *mainLayout = new QVBoxLayout(toolPanelContent); // 在 toolPanelContent 上创建主布局
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // --- 分组1: Basic Objects & Selection ---
    QGroupBox *basicGroup = new QGroupBox(tr("Basic & Select"), toolPanelContent); // 父对象设为 toolPanelContent
    QVBoxLayout *basicLayout = new QVBoxLayout; // 布局会自动设置给父 QGroupBox
    basicLayout->setSpacing(5);
    basicLayout->setContentsMargins(5,5,5,5);

    basicLayout->addWidget(createToolButton(tr("Move/Select"), ":/raw_icons/move.png", tr("Move or select objects")));
    basicLayout->addWidget(createToolButton(tr("Point"), ":/raw_icons/point.png", tr("Draw a point")));
    basicLayout->addWidget(createToolButton(tr("Line"), ":/raw_icons/line.png", tr("Draw a line")));
    basicLayout->addWidget(createToolButton(tr("Segment"), ":/raw_icons/segment.png", tr("Draw a line segment")));
    basicLayout->addStretch();
    basicGroup->setLayout(basicLayout);

    // --- 分组2: Circle Constructions ---
    QGroupBox *circleGroup = new QGroupBox(tr("Circles"), toolPanelContent);
    QVBoxLayout *circleLayout = new QVBoxLayout;
    circleLayout->setSpacing(5);
    circleLayout->setContentsMargins(5,5,5,5);

    circleLayout->addWidget(createToolButton(tr("Arc"), ":/raw_icons/circle_arc.png", tr("Draw a circular arc")));
    circleLayout->addWidget(createToolButton(tr("Circle (Center, Radius)"), ":/raw_icons/circle_center_radius.png", tr("Draw circle with center and radius")));
    circleLayout->addWidget(createToolButton(tr("Circle (Center, Point)"), ":/raw_icons/circle_center_point.png", tr("Draw circle with center and point on circumference")));
    circleLayout->addWidget(createToolButton(tr("Semicircle"), ":/raw_icons/circle_semi.png", tr("Draw a semicircle")));
    circleLayout->addWidget(createToolButton(tr("Circle (3 Points)"), ":/raw_icons/circle_three_point.png", tr("Draw circle through three points")));
    circleLayout->addStretch();
    circleGroup->setLayout(circleLayout);

    // --- 分组3: Geometric Constructions ---
    QGroupBox *constructionGroup = new QGroupBox(tr("Constructions"), toolPanelContent);
    QVBoxLayout *constructionLayout = new QVBoxLayout;
    constructionLayout->setSpacing(5);
    constructionLayout->setContentsMargins(5,5,5,5);
    constructionLayout->addWidget(createToolButton(tr("Midpoint"), ":/raw_icons/midpoint.png", tr("Find midpoint of a segment")));
    constructionLayout->addWidget(createToolButton(tr("Parallel Line"), ":/raw_icons/parrallel.png", tr("Draw a line parallel to another line")));
    constructionLayout->addWidget(createToolButton(tr("Perpendicular Line"), ":/raw_icons/perpendicular.png", tr("Draw a line perpendicular to another line")));
    constructionLayout->addWidget(createToolButton(tr("Perpendicular Bisector"), ":/raw_icons/perpendicular_bisector.png", tr("Draw perpendicular bisector of a segment")));
    constructionLayout->addWidget(createToolButton(tr("Angle Bisector"), ":/raw_icons/angle_bisector.png", tr("Draw angle bisector")));
    constructionLayout->addWidget(createToolButton(tr("Tangents"), ":/raw_icons/tangents.png", tr("Draw tangents")));
    constructionLayout->addStretch();
    constructionGroup->setLayout(constructionLayout);

    // --- 分组4: Transformations & Utilities ---
    QGroupBox *transformUtilGroup = new QGroupBox(tr("Transform & Utils"), toolPanelContent);
    QVBoxLayout *transformUtilLayout = new QVBoxLayout;
    transformUtilLayout->setSpacing(5);
    transformUtilLayout->setContentsMargins(5,5,5,5);
    transformUtilLayout->addWidget(createToolButton(tr("Reflect Point"), ":/raw_icons/reflect_point.png", tr("Reflect point across a line or point")));
    transformUtilLayout->addWidget(createToolButton(tr("Reflect Line"), ":/raw_icons/reflect_line.png", tr("Reflect line across a line or point")));
    transformUtilLayout->addWidget(createToolButton(tr("Reflect Circle"), ":/raw_icons/reflect_circle.png", tr("Reflect circle across a line or point")));
    transformUtilLayout->addWidget(createToolButton(tr("Show/Hide"), ":/raw_icons/show_hide.png", tr("Show or hide objects")));
    transformUtilLayout->addWidget(createToolButton(tr("Delete"), ":/raw_icons/delete.png", tr("Delete objects")));
    transformUtilLayout->addStretch();
    transformUtilGroup->setLayout(transformUtilLayout);


    mainLayout->addWidget(basicGroup);
    mainLayout->addWidget(circleGroup);
    mainLayout->addWidget(constructionGroup);
    mainLayout->addWidget(transformUtilGroup);
    mainLayout->addStretch(); // 确保所有组都靠上

    toolPanelContent->setLayout(mainLayout); // 将主布局设置给 toolPanelContent
}

// 槽函数，响应工具按钮的点击 (保持不变)
void MainWindow::onToolSelected(QAbstractButton *abstractButton)
{
    QToolButton *button = qobject_cast<QToolButton*>(abstractButton);
    if (!button) {
        qWarning("Clicked button is not a QToolButton, skipping.");
        return;
    }

    qDebug() << "工具已选择:" << button->text() << "(Checked:" << button->isChecked() << ")";

    if (!m_canvas) {
        qWarning("Canvas 对象未初始化!");
        return;
    }

    QString toolId = button->property("toolIdentifier").toString();

    if (toolId == tr("Move/Select")) {
        m_canvas->setMode(Canvas::SelectionMode);
        qDebug() << "模式设置为: SelectionMode";
    } else if (toolId == tr("Point")) {
        m_canvas->setMode(Canvas::CreatePointMode);
        qDebug() << "模式设置为: CreatePointMode";
    } else {
        // 对于尚未明确处理的工具，可以设置为 OperationMode 或 SelectionMode
        // 或者在 Canvas 中为每个工具实现一个特定的模式
        m_canvas->setMode(Canvas::OperationMode); // 默认或通用操作模式
        qDebug() << "模式设置为: OperationMode (工具: " << toolId << ")";
    }

    if (toolId == tr("Circle (Center, Radius)") || toolId == tr("Circle (Center, Point)")){
        m_canvas->setOperation(0);
    } else if (toolId == tr("Line")){
        m_canvas->setOperation(1);
    } else if (toolId == tr("Segment")){
        m_canvas->setOperation(2);
    }
}

