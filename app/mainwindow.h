#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingscontainer.h"

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>

class PuzzleWidget;
class PiecesModel;
class StartPuzzleWidget;
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

    enum GameStatus
    {
        GS_InitialLocked,
        GS_LaserPassed,
        GS_LaserFailed,
        GS_TouchAndLaserPassed,
        GS_TouchAndLaserFailed,
        GS_PuzzleCompleted,
        GS_PuzzleTimeIsUp,
    };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openImage(const QString &path = QString());



protected:
    bool event(QEvent *event) override;


private slots:
    void blinkTimeDisplay();
    void updateTimeDisplay();
    void reactOnMediaStateChange();

    // Game State slots
    void setInitialAppState();
    void reactIfLaserPassed();
    void reactIfLaserFailed();
    void reactOnTouchIfLaserPassed();
    void reactOnTouchIfLaserFailed();
    void reactWhenPuzzleIsCompleted();
    void notifyGameOver();

private:
    void initTimer();
    void initSoundPlayer();
    void initTestpointsController();

    void setupWidgets();
    void setupGameFrames();
    void setupTimeWidget();
    void setupPuzzleWidget();
    void setupPuzzleSource();

    void loadSettingsFromFile();

    void setUiLocked(bool locked);

    void setupPuzzle();
    void runPuzzleGame();

private:
    static QString settingsFilePath;
    static QString soundsDirPath;
    static QString imagesDirPath;

    GameStatus _gameStatus;

    QPixmap _puzzleImage;
    QListView *_piecesList;
    PuzzleWidget *_puzzleWidget;
    PiecesModel *_model;

    QFrame *_gameFrame;

    StartPuzzleWidget *_startFrame;
    AccessDeniedWidget *_loseFrame;
    AccessGrantedWidget *_winFrame;
    QStackedWidget *_stackedWidget;

    QLabel *_remainingTimeWidget;

    TestpointsController *_testpointsController;

    QMediaPlayer *_soundPlayer;
    QTimer _puzzleTimer;

    SettingsContainer _settingsContainer;
};

#endif // MAINWINDOW_H
