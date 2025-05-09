// ThreeInOneButton.h
#ifndef THREEINONEBUTTON_H
#define THREEINONEBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class ThreeInOneButton : public QWidget {
    Q_OBJECT

public:
    enum Option {
        SegmentOption = 0,
        ArrayOption = 1,
        LineOption = 2
    };

    ThreeInOneButton(QWidget* parent = nullptr);

    QPushButton* mainButton() const { return m_mainButton; }
    QPushButton* segmentButton() const { return m_optionButtons[SegmentOption]; }
    QPushButton* arrayButton() const { return m_optionButtons[ArrayOption]; }
    QPushButton* lineButton() const { return m_optionButtons[LineOption]; }

    void setActiveOption(Option option);
    Option activeOption() const { return m_activeOption; }

signals:
    void segmentClicked();
    void arrayClicked();
    void lineClicked();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QPushButton* m_mainButton;
    QPushButton* m_optionButtons[3]; // Segment, Array, Line
    QHBoxLayout* m_layout;
    bool m_expanded;
    Option m_activeOption;

    QParallelAnimationGroup* m_expandAnimation;
    QParallelAnimationGroup* m_collapseAnimation;

    void setupAnimations();
    void expand();
    void collapse();
    void updateMainButtonText();
};

#endif // THREEINONEBUTTON_H
