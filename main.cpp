#include "mainwindow.h"

#include <QApplication>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (argc > 1) {
        QString path = QString::fromLocal8Bit(argv[1]);
        if (QFileInfo::exists(path)) {
            w.m_canvas->setFilePath(path);
            w.m_canvas->loadFile(true);
        }
    }

    w.show();
    return a.exec();
}
