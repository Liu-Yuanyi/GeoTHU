// ThreeInOneButton.cpp
#include "ThreeInOneButton.h"
#include <QPropertyAnimation>

ThreeInOneButton::ThreeInOneButton(QWidget* parent)
    : QWidget(parent), m_expanded(false), m_activeOption(SegmentOption)
{
    // Create the main button (defaults to Segment)
    m_mainButton = new QPushButton("Segment", this);

    // Create option buttons
    m_optionButtons[SegmentOption] = new QPushButton("Segment", this);
    m_optionButtons[ArrayOption] = new QPushButton("Array", this);
    m_optionButtons[LineOption] = new QPushButton("Line", this);

    // Make buttons checkable
    m_mainButton->setCheckable(true);
    for (int i = 0; i < 3; i++) {
        m_optionButtons[i]->setCheckable(true);
        m_optionButtons[i]->hide(); // Hide initially
    }

    // Set fixed size for each button
    m_mainButton->setFixedSize(60, 50);
    for (int i = 0; i < 3; i++) {
        m_optionButtons[i]->setFixedSize(60, 50);
    }

    // Create layout
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    m_layout->addWidget(m_mainButton);

    // Add option buttons to layout
    for (int i = 0; i < 3; i++) {
        m_layout->addWidget(m_optionButtons[i]);
    }

    // Set fixed size for the widget
    setFixedSize(60, 50);

    // Connect signals
    connect(m_optionButtons[SegmentOption], &QPushButton::clicked, [this]() {
        setActiveOption(SegmentOption);
        emit segmentClicked();
    });

    connect(m_optionButtons[ArrayOption], &QPushButton::clicked, [this]() {
        setActiveOption(ArrayOption);
        emit arrayClicked();
    });

    connect(m_optionButtons[LineOption], &QPushButton::clicked, [this]() {
        setActiveOption(LineOption);
        emit lineClicked();
    });

    // Connect main button to the active option
    connect(m_mainButton, &QPushButton::clicked, [this]() {
        // Make sure main button is checked
        m_mainButton->setChecked(true);

        // Trigger the currently active option
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

    // Set up animations
    setupAnimations();

    // Default to Segment and ensure it's checked
    setActiveOption(SegmentOption);
    m_mainButton->setChecked(true);
}

void ThreeInOneButton::setActiveOption(Option option) {
    m_activeOption = option;
    updateMainButtonText();

    // Set the main button as checked to make it appear in blue
    m_mainButton->setChecked(true);

    // Make sure the corresponding option button is also checked
    // and other option buttons are unchecked
    for (int i = 0; i < 3; i++) {
        m_optionButtons[i]->setChecked(i == option);
    }
}

void ThreeInOneButton::updateMainButtonText() {
    switch (m_activeOption) {
    case SegmentOption:
        m_mainButton->setText("Segment");
        break;
    case ArrayOption:
        m_mainButton->setText("Array");
        break;
    case LineOption:
        m_mainButton->setText("Line");
        break;
    }
}

void ThreeInOneButton::setupAnimations() {
    // Create animations for expanding
    m_expandAnimation = new QParallelAnimationGroup(this);

    QPropertyAnimation* widthAnimation = new QPropertyAnimation(this, "minimumWidth");
    widthAnimation->setDuration(200);
    widthAnimation->setStartValue(60);
    widthAnimation->setEndValue(60 * 3 + 4); // 3 buttons (2 options + main) + spacing
    m_expandAnimation->addAnimation(widthAnimation);

    QPropertyAnimation* maxWidthAnimation = new QPropertyAnimation(this, "maximumWidth");
    maxWidthAnimation->setDuration(200);
    maxWidthAnimation->setStartValue(60);
    maxWidthAnimation->setEndValue(60 * 3 + 4); // 3 buttons + spacing
    m_expandAnimation->addAnimation(maxWidthAnimation);

    // Create animations for collapsing
    m_collapseAnimation = new QParallelAnimationGroup(this);

    QPropertyAnimation* collapseWidthAnimation = new QPropertyAnimation(this, "minimumWidth");
    collapseWidthAnimation->setDuration(200);
    collapseWidthAnimation->setStartValue(60 * 3 + 4);
    collapseWidthAnimation->setEndValue(60);
    m_collapseAnimation->addAnimation(collapseWidthAnimation);

    QPropertyAnimation* collapseMaxWidthAnimation = new QPropertyAnimation(this, "maximumWidth");
    collapseMaxWidthAnimation->setDuration(200);
    collapseMaxWidthAnimation->setStartValue(60 * 3 + 4);
    collapseMaxWidthAnimation->setEndValue(60);
    m_collapseAnimation->addAnimation(collapseMaxWidthAnimation);
}

void ThreeInOneButton::enterEvent(QEnterEvent* event) {
    Q_UNUSED(event);
    expand();
}

void ThreeInOneButton::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    collapse();
}

void ThreeInOneButton::expand() {
    if (!m_expanded) {
        // Show only the non-active options
        for (int i = 0; i < 3; i++) {
            if (i != m_activeOption) {
                m_optionButtons[i]->show();
            }
        }

        m_expandAnimation->start();
        m_expanded = true;
    }
}

void ThreeInOneButton::collapse() {
    if (m_expanded) {
        m_collapseAnimation->start();
        m_expanded = false;

        // Hide all option buttons after animation
        connect(m_collapseAnimation, &QParallelAnimationGroup::finished, [this]() {
            for (int i = 0; i < 3; i++) {
                m_optionButtons[i]->hide();
            }
        });
    }
}
