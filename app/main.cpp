#include "mainwindow.h"
#include "accessgrantedwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(puzzle);

    QApplication app(argc, argv);
    MainWindow window;

    QString imageName = "puzzle_image.jpg";
    QString imagePath = QApplication::applicationDirPath()
            + QString("/images/%1").arg(imageName);
    window.openImage(imagePath);
    window.show();

    return app.exec();
}
