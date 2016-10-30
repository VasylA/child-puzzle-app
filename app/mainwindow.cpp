#include "mainwindow.h"
#include "piecesmodel.h"
#include "puzzlewidget.h"

#include "startpuzzlewidget.h"
#include "accessgrantedwidget.h"
#include "accessdeniedwidget.h"

#include "testpointscontroller.h"
#include "settingsmanager.h"

#include <QPalette>
#include <QtWidgets>
#include <QApplication>
#include <QLinearGradient>

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

#include <stdlib.h>

QString MainWindow::settingsFilePath = "";
QString MainWindow::soundsDirPath = "";
QString MainWindow::imagesDirPath = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _gameStatus(GS_InitialLocked),
      _piecesList(nullptr),
      _puzzleWidget(nullptr),
      _model(nullptr),
      _gameFrame(nullptr),
      _loseFrame(nullptr),
      _winFrame(nullptr),
      _stackedWidget(nullptr),
      _remainingTimeWidget(nullptr),
      _testpointsController(nullptr),
      _soundPlayer(nullptr)
{
    settingsFilePath = QApplication::applicationDirPath() + "/config.xml";
    soundsDirPath = QApplication::applicationDirPath() + "/sounds/";
    imagesDirPath = QApplication::applicationDirPath() + "/images/";

    loadSettingsFromFile();

    setupWidgets();

    initTimer();

    initSoundPlayer();

    initTestpointsController();

    QString imageName = "puzzle_image.jpg";
    QString imagePath = imagesDirPath + imageName;
    openImage(imagePath);
}

MainWindow::~MainWindow()
{
    setInitialAppState();
}

void MainWindow::openImage(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
    {
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    }

    if (!fileName.isEmpty())
    {
        QPixmap newImage;
        if (!newImage.load(fileName))
        {
            QMessageBox::warning(this, tr("Open Image"),
                                 tr("The image file could not be loaded."),
                                 QMessageBox::Cancel);
            close();
            return;
        }
        _puzzleImage = newImage;

        setInitialAppState();
        _testpointsController->checkInGpioStatus();
    }
}

void MainWindow::blinkTimeDisplay()
{
    QPalette colorScheme(_remainingTimeWidget->palette());
    colorScheme.setBrush(QPalette::WindowText, Qt::transparent);
    _remainingTimeWidget->setPalette(colorScheme);
}

void MainWindow::updateTimeDisplay()
{
    static const int millisecondsPerSecond = 1000;

    int remainingTime = _puzzleTimer.remainingTime() / millisecondsPerSecond;
    QString timeString = QString("<p>Залишилося часу: %0</p>").arg(remainingTime);
    _remainingTimeWidget->setText(timeString);

    int greenChanel = 50;
    int blueChanel = 255 * (double)millisecondsPerSecond * remainingTime / _settingsContainer.gameTimerPeriod; //[255-0]
    int redChanel = 255 -  blueChanel;                                                                         //[0-255]
    QColor textColor = QColor(redChanel, greenChanel, blueChanel);

    QPalette colorScheme(_remainingTimeWidget->palette());
    colorScheme.setBrush(QPalette::WindowText, textColor);
    _remainingTimeWidget->setPalette(colorScheme);

    QTimer::singleShot(1000, this, SLOT(updateTimeDisplay()));
    if (remainingTime < 6)
        QTimer::singleShot(500, this, SLOT(blinkTimeDisplay()));
}

void MainWindow::reactOnMediaStateChange()
{
    QMediaPlayer::State mediaState = _soundPlayer->state();
    bool mediaStopped = (mediaState == QMediaPlayer::StoppedState);

    if (!mediaStopped)
        return;

    switch (_gameStatus)
    {
    case GS_PuzzleTimeIsUp:
        _testpointsController->sendPuzzleIncompeteSignalToOutGpios();
        break;

    default:
        break;
    }
}

void MainWindow::setInitialAppState()
{
    if (_gameStatus == GS_InitialLocked)
        return;

    _gameStatus = GS_InitialLocked;

    if (_stackedWidget->currentWidget() != _startFrame)
        _stackedWidget->setCurrentWidget(_startFrame);

    _puzzleWidget->clear();

    _puzzleTimer.stop();
    _soundPlayer->stop();

    connect(_testpointsController, SIGNAL(laserPassed()), this, SLOT(reactIfLaserPassed()), Qt::UniqueConnection);
    connect(_testpointsController, SIGNAL(laserFailed()), this, SLOT(reactIfLaserFailed()), Qt::UniqueConnection);

    disconnect(_soundPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(reactOnMediaStateChange()));

    _testpointsController->resetOutGpiosStatus();

    setUiLocked(true);
}

void MainWindow::reactIfLaserPassed()
{
    if (_gameStatus != GS_InitialLocked)
        return;

    if (_stackedWidget->currentWidget() != _startFrame)
        _stackedWidget->setCurrentWidget(_startFrame);

    _gameStatus = GS_LaserPassed;

    _puzzleTimer.stop();
    _soundPlayer->stop();

    QMediaPlaylist *playlist = new QMediaPlaylist(_soundPlayer);
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "laser.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    playlist->setCurrentIndex(0);

    _soundPlayer->setPlaylist(playlist);
    _soundPlayer->play();

    _testpointsController->resetOutGpiosStatus();

    setUiLocked(false);
}

void MainWindow::reactIfLaserFailed()
{
    if ( (_gameStatus != GS_InitialLocked) &&
         (_gameStatus != GS_LaserPassed) )
        return;

    if (_stackedWidget->currentWidget() != _startFrame)
        _stackedWidget->setCurrentWidget(_startFrame);

    _gameStatus = GS_LaserFailed;

    _puzzleTimer.stop();
    _soundPlayer->stop();

    QMediaPlaylist *playlist = new QMediaPlaylist(_soundPlayer);
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "laser_fail.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    playlist->setCurrentIndex(0);

    _soundPlayer->setPlaylist(playlist);
    _soundPlayer->play();

    _testpointsController->resetOutGpiosStatus();

    setUiLocked(false);
}

void MainWindow::reactOnTouchIfLaserPassed()
{
    if (_gameStatus != GS_LaserPassed)
        return;

    disconnect(_testpointsController, SIGNAL(laserPassed()), this, SLOT(reactIfLaserPassed()));
    disconnect(_testpointsController, SIGNAL(laserFailed()), this, SLOT(reactIfLaserFailed()));

    if (_stackedWidget->currentWidget() != _gameFrame)
        _stackedWidget->setCurrentWidget(_gameFrame);

    _gameStatus = GS_TouchAndLaserPassed;

    _puzzleTimer.stop();
    _soundPlayer->stop();

    QMediaPlaylist *playlist = new QMediaPlaylist(_soundPlayer);
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "laser_pass.mp3"));
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "clock.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    playlist->setCurrentIndex(0);

    _soundPlayer->setPlaylist(playlist);
    _soundPlayer->play();

    _testpointsController->resetOutGpiosStatus();

    setUiLocked(false);

    runPuzzleGame();
}

void MainWindow::reactOnTouchIfLaserFailed()
{
    if (_gameStatus != GS_LaserFailed)
        return;

    disconnect(_testpointsController, SIGNAL(laserPassed()), this, SLOT(reactIfLaserPassed()));
    disconnect(_testpointsController, SIGNAL(laserFailed()), this, SLOT(reactIfLaserFailed()));

    if (_stackedWidget->currentWidget() != _gameFrame)
        _stackedWidget->setCurrentWidget(_gameFrame);

    _gameStatus = GS_TouchAndLaserFailed;

    _puzzleTimer.stop();
    _soundPlayer->stop();

    QMediaPlaylist *playlist = new QMediaPlaylist(_soundPlayer);
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "clock.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    playlist->setCurrentIndex(0);

    _soundPlayer->setPlaylist(playlist);
    _soundPlayer->play();

    _testpointsController->resetOutGpiosStatus();

    setUiLocked(false);

    runPuzzleGame();
}

void MainWindow::reactWhenPuzzleIsCompleted()
{
    if ( (_gameStatus != GS_TouchAndLaserFailed) &&
         (_gameStatus != GS_TouchAndLaserPassed) )
        return;

    _gameStatus = GS_PuzzleCompleted;

    _puzzleTimer.stop();
    _soundPlayer->stop();

    _stackedWidget->setCurrentWidget(_winFrame);

    QMediaPlaylist *playlist = new QMediaPlaylist(_soundPlayer);
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "puzzle_pass.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    playlist->setCurrentIndex(0);

    _soundPlayer->setPlaylist(playlist);
    _soundPlayer->play();

    _testpointsController->sendPuzzleCompeteSignalToOutGpios();

    QTimer::singleShot(_settingsContainer.gameResetPeriod, this, SLOT(setInitialAppState()));
}

void MainWindow::notifyGameOver()
{
    if ( (_gameStatus != GS_TouchAndLaserFailed) &&
         (_gameStatus != GS_TouchAndLaserPassed) )
        return;

    _gameStatus = GS_PuzzleTimeIsUp;

    _puzzleTimer.stop();
    _soundPlayer->stop();

    _stackedWidget->setCurrentWidget(_loseFrame);

    QMediaPlaylist *playlist = new QMediaPlaylist(_soundPlayer);
    playlist->addMedia(QUrl::fromLocalFile(soundsDirPath + "puzzle_fail.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    playlist->setCurrentIndex(0);

    connect(_soundPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(reactOnMediaStateChange()), Qt::UniqueConnection);

    _soundPlayer->setPlaylist(playlist);
    _soundPlayer->play();

    QTimer::singleShot(_settingsContainer.gameResetPeriod, this, SLOT(setInitialAppState()));
}

void MainWindow::initTimer()
{
    _puzzleTimer.setInterval(_settingsContainer.gameTimerPeriod);
    _puzzleTimer.setSingleShot(false);

    connect(&_puzzleTimer, SIGNAL(timeout()), SLOT(notifyGameOver()));
}

void MainWindow::initSoundPlayer()
{
    _soundPlayer = new QMediaPlayer(this);
    _soundPlayer->setVolume(80);
}

void MainWindow::initTestpointsController()
{
    _testpointsController = new TestpointsController(this);

    connect(_testpointsController, SIGNAL(initialAppStateRequested()), this, SLOT(setInitialAppState()));
    connect(_testpointsController, SIGNAL(laserPassed()), this, SLOT(reactIfLaserPassed()), Qt::UniqueConnection);
    connect(_testpointsController, SIGNAL(laserFailed()), this, SLOT(reactIfLaserFailed()), Qt::UniqueConnection);
}

void MainWindow::setupPuzzle()
{
    QRect imageRect = _puzzleImage.rect();
    QPixmap currentPixmap = _puzzleImage.copy(imageRect);

    QSize imageSize = _puzzleWidget->imageSize();
    _puzzleImage = currentPixmap.scaled(imageSize.width(),
                                        imageSize.height(),
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);

    qsrand(QCursor::pos().x() ^ QCursor::pos().y());

    _model->addPieces(_puzzleImage);
    _puzzleWidget->clear();
}

void MainWindow::runPuzzleGame()
{
    setupPuzzle();

    _puzzleTimer.start(_settingsContainer.gameTimerPeriod);
    updateTimeDisplay();
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        switch (_gameStatus )
        {
        case GS_LaserPassed:
            reactOnTouchIfLaserPassed();
            break;

        case GS_LaserFailed:
            reactOnTouchIfLaserFailed();
            break;

        default:
            break;
        }
        setCursor(Qt::BlankCursor);
        //falldown

    default:
        return QWidget::event(event);
    }
    return true;
}

void MainWindow::setupWidgets()
{
    //NOTE: Setup puzzle widget before puzzle source (_piecesList)
    setupPuzzleWidget();
    setupPuzzleSource();

    setupTimeWidget();

    setupGameFrames();

    _stackedWidget = new QStackedWidget;
    _stackedWidget->addWidget(_startFrame);
    _stackedWidget->addWidget(_gameFrame);
    _stackedWidget->addWidget(_loseFrame);
    _stackedWidget->addWidget(_winFrame);

    setCentralWidget(_stackedWidget);

    QLinearGradient bgGradient;
    bgGradient.setStart(0, 0);
    bgGradient.setFinalStop(0, 1);
    bgGradient.setColorAt(0.0, QColor(120, 230, 180));
    bgGradient.setColorAt(0.2, QColor(120, 180, 180));
    bgGradient.setColorAt(1.0, QColor(120, 180, 120));
    bgGradient.setCoordinateMode(QGradient::StretchToDeviceMode);

    QPalette colorScheme(palette());
    colorScheme.setBrush(QPalette::Background, bgGradient);

    setAutoFillBackground(true);
    setPalette(colorScheme);

    setCursor(Qt::BlankCursor);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setWindowState(Qt::WindowFullScreen);
    setWindowTitle(tr("Puzzle"));
}

void MainWindow::setupGameFrames()
{
    QSizeF availableScreenSize = qApp->primaryScreen()->availableSize();

    QHBoxLayout *remaininTimeLayout = new QHBoxLayout;
    remaininTimeLayout->addSpacing(availableScreenSize.width() / 8);
    remaininTimeLayout->addWidget(_remainingTimeWidget);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(remaininTimeLayout);
    rightLayout->addWidget(_puzzleWidget);

    _gameFrame = new QFrame;
    QHBoxLayout *gameLayout = new QHBoxLayout(_gameFrame);
    gameLayout->addLayout(rightLayout);
    gameLayout->addWidget(_piecesList);

    const int TEXT_PIXEL_SIZE = availableScreenSize.height() / 5;

    _startFrame = new StartPuzzleWidget;
    _startFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _startFrame->setTextSize(TEXT_PIXEL_SIZE);

    _winFrame = new AccessGrantedWidget;
    _winFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _winFrame->setTextSize(TEXT_PIXEL_SIZE);

    _loseFrame = new AccessDeniedWidget;
    _loseFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _loseFrame->setTextSize(TEXT_PIXEL_SIZE);
}

void MainWindow::setupTimeWidget()
{
    _remainingTimeWidget = new QLabel;

    QSizeF availableScreenSize = qApp->primaryScreen()->availableSize();

    QFont labelFont = _remainingTimeWidget->font();
    labelFont.setPixelSize(availableScreenSize.height() / 20);
    labelFont.setBold(true);
    labelFont.setFamily("Verdana");
    _remainingTimeWidget->setFont(labelFont);
}

void MainWindow::setupPuzzleWidget()
{
    const double margins = 10;
    QSizeF availableScreenSize = qApp->primaryScreen()->availableSize();

    double imageWidth = 0.8 * availableScreenSize.width() - 2 * margins;
    double imageHeight = 0.9 * availableScreenSize.height() - 2 * margins;

    _puzzleWidget = new PuzzleWidget(_settingsContainer.rowCount,
                                     _settingsContainer.columnCount,
                                     QSize(imageWidth, imageHeight));

    QString backgroundImageName = "puzzle_background.jpg";
    QString backgroundImagePath = imagesDirPath + backgroundImageName;
    _puzzleWidget->setBackgroundImage(backgroundImagePath);

    connect(_puzzleWidget, SIGNAL(puzzleCompleted()),
            this, SLOT(reactWhenPuzzleIsCompleted()), Qt::QueuedConnection);
}

void MainWindow::setupPuzzleSource()
{
    QSizeF availableScreenSize = qApp->primaryScreen()->availableSize();

    double gridWidth = 0.2 * availableScreenSize.width();
    double iconWidth = 0.9 * gridWidth;

    QSizeF iconSize(iconWidth, iconWidth);
    QSizeF gridSize(gridWidth, gridWidth);

    _piecesList = new QListView;
    _piecesList->setDragEnabled(true);
    _piecesList->setViewMode(QListView::IconMode);
    _piecesList->setWrapping(true);

    _piecesList->setIconSize(iconSize.toSize());
    _piecesList->setGridSize(gridSize.toSize());
    _piecesList->setSpacing(10);

    _piecesList->setMovement(QListView::Snap);
    _piecesList->setAcceptDrops(true);
    _piecesList->setDropIndicatorShown(true);

    _piecesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _piecesList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _model = new PiecesModel(_settingsContainer.columnCount,
                             _settingsContainer.rowCount,
                             _puzzleWidget->pieceSize(),
                             this);
    _piecesList->setModel(_model);

    QPalette puzzleSourcePalette = _piecesList->palette();
    puzzleSourcePalette.setBrush(QPalette::Base, Qt::lightGray);
    _piecesList->setAutoFillBackground(true);
    _piecesList->setPalette(puzzleSourcePalette);
}

void MainWindow::loadSettingsFromFile()
{
    SettingsManager gameSettingsManager;
    if (gameSettingsManager.loadSettings(settingsFilePath))
        _settingsContainer = gameSettingsManager.settings();
}

void MainWindow::setUiLocked(bool locked)
{
    setAttribute(Qt::WA_AcceptTouchEvents, !locked);
    setAttribute(Qt::WA_StaticContents, !locked);
    setDisabled(locked);
}
