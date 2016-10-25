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
    void playMusic();

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
    QLabel *_loseFrame;
    AccessGrantedWidget *_winFrame;
    //--------------------------------------

    static const int GAME_TIMER_PERIOD = 10000;
    static const int GAME_RESET_PERIOD = 15000;
    static const int PIECE_COUNT_BY_SIDE = 2;
};

#endif // MAINWINDOW_H
