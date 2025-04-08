/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action2233;
    QAction *action232334;
    QAction *action2324r2;
    QWidget *centralwidget;
    QLabel *mylabel;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        action2233 = new QAction(MainWindow);
        action2233->setObjectName("action2233");
        action232334 = new QAction(MainWindow);
        action232334->setObjectName("action232334");
        action2324r2 = new QAction(MainWindow);
        action2324r2->setObjectName("action2324r2");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mylabel = new QLabel(centralwidget);
        mylabel->setObjectName("mylabel");
        mylabel->setGeometry(QRect(270, 260, 58, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menu->addAction(action2233);
        menu->addAction(action232334);
        menu->addSeparator();
        menu->addAction(action2324r2);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        action2233->setText(QCoreApplication::translate("MainWindow", "2233", nullptr));
        action232334->setText(QCoreApplication::translate("MainWindow", "232334", nullptr));
        action2324r2->setText(QCoreApplication::translate("MainWindow", "2324r2", nullptr));
        mylabel->setText(QCoreApplication::translate("MainWindow", "a", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "HHHhh", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
