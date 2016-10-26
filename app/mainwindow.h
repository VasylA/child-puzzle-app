#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingscontainer.h"

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
    void updateTimeDisplay();

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

    QMediaPlayer *_soundPlayer;
    QTimer _puzzleTimer;

    SettingsContainer _settingsContainer;

    static const QString settingsFilePath;
    static const int TEXT_PIXEL_SIZE = 120;
};

#endif // MAINWINDOW_H
