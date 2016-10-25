#include "mainwindow.h"
#include "accessgrantedwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(puzzle);

    QApplication app(argc, argv);
    MainWindow window;
    window.openImage(":/images/puzzleImage.jpg");
    window.show();
    return app.exec();
}
