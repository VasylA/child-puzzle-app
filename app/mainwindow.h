#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>

class PuzzleWidget;
class PiecesModel;
QT_BEGIN_NAMESPACE
class QListView;
QT_END_NAMESPACE
class QStackedWidget;
class QMediaPlayer;
class QLCDNumber;
class QFrame;
class QLabel;

class AccessGrantedWidget;
class AccessDeniedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void openImage(const QString &path = QString());
    void setupPuzzle();

private slots:
    void gameOver();
    void resetPuzzle();
    void setCompleted();
    void updateLcdTime();

private:
    void setupTimer();
    void setupWidgets();

    QPixmap _puzzleImage;
    QListView *_piecesList;
    PuzzleWidget *_puzzleWidget;
    PiecesModel *_model;

    QMediaPlayer *_soundPlayer;

    QLCDNumber *_remainingTimeWidget;
    QTimer _puzzleTimer;

    //--------------------------------------
    QStackedWidget *_stackedWidget;

    QFrame *_gameFrame;
    AccessDeniedWidget *_loseFrame;
    AccessGrantedWidget *_winFrame;
    //--------------------------------------

    static const int GAME_TIMER_PERIOD = 15000;
    static const int GAME_RESET_PERIOD = 10000;
    static const int PIECE_COUNT_IN_COLUMN = 2;
    static const int PIECE_COUNT_IN_ROW = 3;
    static const int TEXT_PIXEL_SIZE = 120;
};

#endif // MAINWINDOW_H
