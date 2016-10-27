#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingscontainer.h"

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>

class PuzzleWidget;
class PiecesModel;
class AccessGrantedWidget;
class AccessDeniedWidget;
class TestpointsController;

QT_BEGIN_NAMESPACE
class QListView;
class QStackedWidget;
class QMediaPlayer;
class QLCDNumber;
class QFrame;
class QLabel;
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
    void gameOver();
    void resetPuzzle();
    void setCompleted();
    void blinkTimeDisplay();
    void updateTimeDisplay();
    void freezeApplication();

private:
    void setupTimer();
    void setupWidgets();
    void setupGameFrame();
    void setupPuzzleSource();
    void loadSettingsFromFile();

    QPixmap _puzzleImage;
    QListView *_piecesList;
    PuzzleWidget *_puzzleWidget;
    PiecesModel *_model;

    QFrame *_gameFrame;
    AccessDeniedWidget *_loseFrame;
    AccessGrantedWidget *_winFrame;
    QStackedWidget *_stackedWidget;

    QLabel *_remainingTimeWidget;

    TestpointsController *_testpointsController;

    QMediaPlayer *_soundPlayer;
    QTimer _puzzleTimer;

    SettingsContainer _settingsContainer;

    static const QString settingsFilePath;
    static const int TEXT_PIXEL_SIZE = 120;
};

#endif // MAINWINDOW_H
