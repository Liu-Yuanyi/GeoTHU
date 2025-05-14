// ThreeInOneButton.cpp
#include "ThreeInOneButton.h"
#include <QPropertyAnimation> // 用于属性动画

ThreeInOneButton::ThreeInOneButton(QWidget* parent)
    : QWidget(parent), m_expanded(false), m_activeOption(SegmentOption) // 初始化：未展开，默认激活线段选项
{
    // 创建主按钮 (默认为线段)
    m_mainButton = new QPushButton("线段", this); // 初始文本为 "线段"

    // 创建选项按钮
    m_optionButtons[SegmentOption] = new QPushButton("线段", this);
    m_optionButtons[ArrayOption] = new QPushButton("射线", this);   // "Array" 对应 "射线"
    m_optionButtons[LineOption] = new QPushButton("直线", this);

    // 使按钮可选中 (checkable)
    m_mainButton->setCheckable(true);
    for (int i = 0; i < 3; i++) {
        m_optionButtons[i]->setCheckable(true);
        m_optionButtons[i]->hide(); // 初始时隐藏选项按钮
    }

    // 为每个按钮设置固定大小
    m_mainButton->setFixedSize(60, 50);
    for (int i = 0; i < 3; i++) {
        m_optionButtons[i]->setFixedSize(60, 50);
    }

    // 创建布局
    m_layout = new QHBoxLayout(this); // 水平布局
    m_layout->setContentsMargins(0, 0, 0, 0); // 无边距
    m_layout->setSpacing(2); // 按钮间距为 2
    m_layout->addWidget(m_mainButton); // 首先添加主按钮

    // 将选项按钮添加到布局中 (初始时它们是隐藏的)
    for (int i = 0; i < 3; i++) {
        m_layout->addWidget(m_optionButtons[i]);
    }

    // 设置此复合控件的固定大小 (初始为单个按钮的大小)
    setFixedSize(60, 50);

    // 连接信号与槽
    connect(m_optionButtons[SegmentOption], &QPushButton::clicked, [this]() {
        setActiveOption(SegmentOption); // 设置当前激活选项为线段
        emit segmentClicked();          // 发出线段点击信号
    });

    connect(m_optionButtons[ArrayOption], &QPushButton::clicked, [this]() {
        setActiveOption(ArrayOption);   // 设置当前激活选项为射线
        emit arrayClicked();            // 发出射线点击信号
    });

    connect(m_optionButtons[LineOption], &QPushButton::clicked, [this]() {
        setActiveOption(LineOption);    // 设置当前激活选项为直线
        emit lineClicked();             // 发出直线点击信号
    });

    // 连接主按钮的点击事件到当前激活的选项对应的操作
    connect(m_mainButton, &QPushButton::clicked, [this]() {
        // 确保主按钮处于选中状态 (视觉上)
        m_mainButton->setChecked(true);

        // 根据当前激活的选项触发相应的信号
        switch (m_activeOption) {
        case SegmentOption:
            emit segmentClicked();
            break;
        case ArrayOption:
            emit arrayClicked();
            break;
        case LineOption:
            emit lineClicked();
            break;
        }
    });

    // 设置动画效果
    setupAnimations();

    // 默认设置为线段选项，并确保主按钮已选中
    setActiveOption(SegmentOption);
    m_mainButton->setChecked(true);
}

void ThreeInOneButton::setActiveOption(Option option) {
    m_activeOption = option;    // 更新当前激活的选项
    updateMainButtonText();     // 更新主按钮上显示的文本

    // 将主按钮设置为选中状态，使其显示为蓝色 (或其他选中样式)
    m_mainButton->setChecked(true);

    // 确保对应的选项按钮也被选中，而其他选项按钮未被选中
    for (int i = 0; i < 3; i++) {
        m_optionButtons[i]->setChecked(i == option);
    }
}

void ThreeInOneButton::updateMainButtonText() {
    // 根据当前激活的选项更新主按钮的文本
    switch (m_activeOption) {
    case SegmentOption:
        m_mainButton->setText("线段");
        break;
    case ArrayOption:
        m_mainButton->setText("射线");
        break;
    case LineOption:
        m_mainButton->setText("直线");
        break;
    }
}

void ThreeInOneButton::setupAnimations() {
    // 创建展开动画组
    m_expandAnimation = new QParallelAnimationGroup(this);

    // 宽度动画 (最小宽度)
    QPropertyAnimation* widthAnimation = new QPropertyAnimation(this, "minimumWidth");
    widthAnimation->setDuration(200); // 动画时长 200ms
    widthAnimation->setStartValue(60); // 初始宽度 (一个按钮)
    widthAnimation->setEndValue(60 * 3 + 2 * 2); // 结束宽度 (三个按钮宽度 + 两个间距)
        // (主按钮 + 2个其他选项按钮 = 3个按钮)
    m_expandAnimation->addAnimation(widthAnimation);

    // 宽度动画 (最大宽度)
    QPropertyAnimation* maxWidthAnimation = new QPropertyAnimation(this, "maximumWidth");
    maxWidthAnimation->setDuration(200);
    maxWidthAnimation->setStartValue(60);
    maxWidthAnimation->setEndValue(60 * 3 + 2 * 2); // 同上
    m_expandAnimation->addAnimation(maxWidthAnimation);

    // 创建收起动画组
    m_collapseAnimation = new QParallelAnimationGroup(this);

    // 收起时的宽度动画 (最小宽度)
    QPropertyAnimation* collapseWidthAnimation = new QPropertyAnimation(this, "minimumWidth");
    collapseWidthAnimation->setDuration(200);
    collapseWidthAnimation->setStartValue(60 * 3 + 2 * 2); // 从展开宽度开始
    collapseWidthAnimation->setEndValue(60);             // 收回到单个按钮宽度
    m_collapseAnimation->addAnimation(collapseWidthAnimation);

    // 收起时的宽度动画 (最大宽度)
    QPropertyAnimation* collapseMaxWidthAnimation = new QPropertyAnimation(this, "maximumWidth");
    collapseMaxWidthAnimation->setDuration(200);
    collapseMaxWidthAnimation->setStartValue(60 * 3 + 2 * 2);
    collapseMaxWidthAnimation->setEndValue(60);
    m_collapseAnimation->addAnimation(collapseMaxWidthAnimation);
}

void ThreeInOneButton::enterEvent(QEnterEvent* event) {
    Q_UNUSED(event); // 标记事件参数未使用
    expand();        // 鼠标进入时展开
}

void ThreeInOneButton::leaveEvent(QEvent* event) {
    Q_UNUSED(event); // 标记事件参数未使用
    collapse();      // 鼠标离开时收起
}

void ThreeInOneButton::expand() {
    if (!m_expanded) { // 如果尚未展开
        // 仅显示非当前激活的选项按钮 (当前激活的选项由主按钮代表)
        for (int i = 0; i < 3; i++) {
            if (i != m_activeOption) { // 不显示代表当前激活选项的那个按钮
                m_optionButtons[i]->show();
            } else {
                m_optionButtons[i]->hide(); // 确保代表当前激活选项的按钮是隐藏的
            }
        }

        m_expandAnimation->start(); // 开始展开动画
        m_expanded = true;          // 更新展开状态
    }
}

void ThreeInOneButton::collapse() {
    if (m_expanded) { // 如果已展开
        m_collapseAnimation->start(); // 开始收起动画
        m_expanded = false;           // 更新展开状态

        // 动画结束后隐藏所有选项按钮
        // 使用 disconnectPrevious检查是否已连接，避免重复连接
        disconnect(m_collapseAnimation, &QParallelAnimationGroup::finished, this, nullptr); // 先断开旧连接
        connect(m_collapseAnimation, &QParallelAnimationGroup::finished, [this]() {
            for (int i = 0; i < 3; i++) {
                m_optionButtons[i]->hide();
            }
        });
    }
}
