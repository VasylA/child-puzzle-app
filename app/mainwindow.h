#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

class PuzzleWidget;
class PiecesModel;
QT_BEGIN_NAMESPACE
class QListView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void openImage(const QString &path = QString());
    void setupPuzzle();

private slots:
    void setCompleted();

private:
    void setupWidgets();

    QPixmap _puzzleImage;
    QListView *_piecesList;
    PuzzleWidget *_puzzleWidget;
    PiecesModel *_model;

    static const int PIECE_COUNT_BY_SIDE = 2;
};

#endif // MAINWINDOW_H
