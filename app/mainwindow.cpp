#include "mainwindow.h"
#include "piecesmodel.h"
#include "puzzlewidget.h"

#include "accessgrantedwidget.h"
#include "accessdeniedwidget.h"

#include "testpointscontroller.h"
#include "settingsmanager.h"

#include <QPalette>
#include <QtWidgets>
#include <QApplication>
#include <QLinearGradient>

#include <QtMultimedia/QMediaPlayer>

#include <stdlib.h>

QString MainWindow::settingsFilePath = "";
QString MainWindow::soundsDirPath = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
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

    loadSettingsFromFile();

    setupWidgets();
    _model = new PiecesModel(_settingsContainer.columnCount,
                             _settingsContainer.rowCount,
                             _puzzleWidget->pieceSize(),
                             this);
    _piecesList->setModel(_model);

    _soundPlayer = new QMediaPlayer(this);
    _soundPlayer->setVolume(100);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setWindowState(Qt::WindowFullScreen);
    setWindowTitle(tr("Puzzle"));

    setupTimer();

    _testpointsController = new TestpointsController;
    connect(_testpointsController, SIGNAL(appFreezeRequested()), this, SLOT(freezeApplication()));
    //TODO: Update reaction on next signals
    connect(_testpointsController, SIGNAL(signal1()), this, SLOT(reaction1()));
    connect(_testpointsController, SIGNAL(signal2()), this, SLOT(reaction2()));
}

MainWindow::~MainWindow()
{
    resetPuzzle();
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

        resetPuzzle();
    }
}

void MainWindow::setCompleted()
{
    _puzzleTimer.stop();

    _testpointsController->sendPuzzleCompeteSignalToOutGpios();

    _soundPlayer->stop();
    _soundPlayer->setMedia(QUrl::fromLocalFile(soundsDirPath + "puzzle_pass.mp3"));
    _soundPlayer->play();

    _stackedWidget->setCurrentWidget(_winFrame);

    QTimer::singleShot(_settingsContainer.gameResetPeriod, this, SLOT(resetPuzzle()));
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
    QString timeString = QString("<p style='font-size:60px; font-family:verdana'><b>Залишилося часу: %0</b></p>").arg(remainingTime);
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

void MainWindow::freezeApplication()
{
    //TODO: Implenent this
}

void MainWindow::setupTimer()
{
    _puzzleTimer.setInterval(_settingsContainer.gameTimerPeriod);
    _puzzleTimer.setSingleShot(false);

    connect(&_puzzleTimer, SIGNAL(timeout()), SLOT(gameOver()));
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

void MainWindow::gameOver()
{
    _puzzleTimer.stop();

    _testpointsController->sendPuzzleIncompeteSignalToOutGpios();

    _soundPlayer->stop();
    _soundPlayer->setMedia(QUrl::fromLocalFile(soundsDirPath + "puzzle_fail.mp3"));
    _soundPlayer->play();

    _stackedWidget->setCurrentWidget(_loseFrame);

    QTimer::singleShot(_settingsContainer.gameResetPeriod, this, SLOT(resetPuzzle()));
}

void MainWindow::resetPuzzle()
{
    _puzzleTimer.stop();

    _soundPlayer->stop();
    _soundPlayer->setMedia(QUrl::fromLocalFile(soundsDirPath + "clock.mp3"));
    _soundPlayer->play();

    _testpointsController->resetOutGpiosStatus();

    setupPuzzle();
    _stackedWidget->setCurrentWidget(_gameFrame);

    _puzzleTimer.start(_settingsContainer.gameTimerPeriod);
    updateTimeDisplay();
}

void MainWindow::setupWidgets()
{
    const double margins = 10;
    QSizeF availableScreenSize = qApp->primaryScreen()->availableSize();

    double imageWidth = 0.8 * availableScreenSize.width() - 2 * margins;
    double imageHeight = 0.9 * availableScreenSize.height() - 2 * margins;

    _puzzleWidget = new PuzzleWidget(_settingsContainer.rowCount,
                                     _settingsContainer.columnCount,
                                     QSize(imageWidth, imageHeight));

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

    PiecesModel *model = new PiecesModel(_settingsContainer.columnCount,
                                         _settingsContainer.rowCount,
                                         _puzzleWidget->pieceSize(),
                                         this);
    _piecesList->setModel(model);

    QPalette puzzleSourcePalette = _piecesList->palette();
    puzzleSourcePalette.setBrush(QPalette::Base, Qt::lightGray);
    _piecesList->setAutoFillBackground(true);
    _piecesList->setPalette(puzzleSourcePalette);


    connect(_puzzleWidget, SIGNAL(puzzleCompleted()),
            this, SLOT(setCompleted()), Qt::QueuedConnection);

    _remainingTimeWidget = new QLabel;

    QHBoxLayout *remaininTimeLayout = new QHBoxLayout;
    remaininTimeLayout->addSpacing(100);
    remaininTimeLayout->addWidget(_remainingTimeWidget);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(remaininTimeLayout);
    rightLayout->addWidget(_puzzleWidget);

    _gameFrame = new QFrame;
    QHBoxLayout *gameLayout = new QHBoxLayout(_gameFrame);
    gameLayout->addLayout(rightLayout);
    gameLayout->addWidget(_piecesList);


    _winFrame = new AccessGrantedWidget;
    _winFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _winFrame->setTextSize(TEXT_PIXEL_SIZE);

    _loseFrame = new AccessDeniedWidget;
    _loseFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _loseFrame->setTextSize(TEXT_PIXEL_SIZE);

    _stackedWidget = new QStackedWidget;
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
}

void MainWindow::setupGameFrame()
{
    //TODO: Implenent this
}

void MainWindow::setupPuzzleSource()
{
    //TODO: Implenent this
}

void MainWindow::loadSettingsFromFile()
{
    SettingsManager gameSettingsManager;
    if (gameSettingsManager.loadSettings(settingsFilePath))
        _settingsContainer = gameSettingsManager.settings();
}
