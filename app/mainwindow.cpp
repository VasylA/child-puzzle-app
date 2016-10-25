
#include "mainwindow.h"
#include "piecesmodel.h"
#include "puzzlewidget.h"

#include "accessgrantedwidget.h"
#include "accessdeniedwidget.h"

#include <QtWidgets>
#include <QApplication>

#include <QtMultimedia/QMediaPlayer>
//#include <QSound>

#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _piecesList(nullptr),
      _puzzleWidget(nullptr),
      _model(nullptr)
{
    setupWidgets();
    _model = new PiecesModel(PIECE_COUNT_IN_ROW, PIECE_COUNT_IN_COLUMN, _puzzleWidget->pieceSize(), this);
    _piecesList->setModel(_model);

    _soundPlayer = new QMediaPlayer(this);
    _soundPlayer->setVolume(100);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setWindowState(Qt::WindowFullScreen);
    setWindowTitle(tr("Puzzle"));

    setupTimer();
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
        if (!newImage.load(fileName)) {
            QMessageBox::warning(this, tr("Open Image"),
                                 tr("The image file could not be loaded."),
                                 QMessageBox::Cancel);
            return;
        }
        _puzzleImage = newImage;

        resetPuzzle();
    }
}

void MainWindow::setCompleted()
{
    _puzzleTimer.stop();

    _stackedWidget->setCurrentWidget(_winFrame);

    _soundPlayer->stop();
    _soundPlayer->setMedia(QUrl::fromLocalFile("/home/pi/Documents/Projects/child-puzzle-app/app/win_sound.mp3"));
    _soundPlayer->play();

    //    playMusic();

    QTimer::singleShot(GAME_RESET_PERIOD, this, SLOT(resetPuzzle()));
}

void MainWindow::updateLcdTime()
{
    _remainingTimeWidget->display(_puzzleTimer.remainingTime() / 1000);
    QTimer::singleShot(1000, this, SLOT(updateLcdTime()));
}

void MainWindow::setupTimer()
{
    _puzzleTimer.setInterval(GAME_TIMER_PERIOD);
    _puzzleTimer.setSingleShot(false);

    connect(&_puzzleTimer, SIGNAL(timeout()), SLOT(gameOver()));
}

void MainWindow::setupPuzzle()
{
    int size = qMin(_puzzleImage.width(), _puzzleImage.height());
    QRect imageRect((_puzzleImage.width() - size) / 2,
                    (_puzzleImage.height() - size) / 2,
                    size, size);
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

    _soundPlayer->stop();
    _soundPlayer->setMedia(QUrl::fromLocalFile("/home/pi/Documents/Projects/child-puzzle-app/app/lose_sound.mp3"));
    _soundPlayer->play();

    _stackedWidget->setCurrentWidget(_loseFrame);

    QTimer::singleShot(GAME_RESET_PERIOD, this, SLOT(resetPuzzle()));
}

void MainWindow::resetPuzzle()
{
    _puzzleTimer.stop();
    _soundPlayer->stop();

    setupPuzzle();
    _stackedWidget->setCurrentWidget(_gameFrame);

    _puzzleTimer.start(GAME_TIMER_PERIOD);
    updateLcdTime();


//    _soundPlayer->setMedia(QUrl::fromLocalFile("qrc:/sound/win_sound.mp3"));
//    _soundPlayer->play();
}

void MainWindow::setupWidgets()
{
    const int MARGINS = 20;
    QSize availableScreenSize = qApp->primaryScreen()->availableSize();

    int imageWidth = 0.75 * availableScreenSize.width() - 2 * MARGINS;
    int imageHeight = 0.85 * availableScreenSize.height() - 2 * MARGINS;

    _puzzleWidget = new PuzzleWidget(PIECE_COUNT_IN_COLUMN, PIECE_COUNT_IN_ROW, QSize(imageWidth, imageHeight));

    static const int offset = 20;
    QSize pieceSize = _puzzleWidget->pieceSize();

    QSize iconSize(pieceSize.width() - offset, pieceSize.height() - offset);
    QSize gridSize = pieceSize;

    _piecesList = new QListView;
    _piecesList->setDragEnabled(true);
    _piecesList->setViewMode(QListView::IconMode);

//    _piecesList->setFixedWidth(pieceSize + 2 * offset);
    _piecesList->setIconSize(iconSize);
    _piecesList->setGridSize(gridSize);
    _piecesList->setSpacing(10);

    _piecesList->setMovement(QListView::Snap);
    _piecesList->setAcceptDrops(true);
    _piecesList->setDropIndicatorShown(true);

    PiecesModel *model = new PiecesModel(PIECE_COUNT_IN_ROW,
                                         PIECE_COUNT_IN_COLUMN,
                                         _puzzleWidget->pieceSize(), this);
    _piecesList->setModel(model);

    connect(_puzzleWidget, SIGNAL(puzzleCompleted()),
            this, SLOT(setCompleted()), Qt::QueuedConnection);

    _remainingTimeWidget = new QLCDNumber;

    QHBoxLayout *remaininTimeLayout = new QHBoxLayout;
    remaininTimeLayout->addWidget(new QLabel("<h1><center>Time:</center></h1>"));
    remaininTimeLayout->addWidget(_remainingTimeWidget);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(remaininTimeLayout);
    rightLayout->addWidget(_puzzleWidget);

    _gameFrame = new QFrame;
    QHBoxLayout *gameLayout = new QHBoxLayout(_gameFrame);
    gameLayout->addWidget(_piecesList);
    gameLayout->addLayout(rightLayout);


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
}

void MainWindow::playMusic()
{
//    QMediaPlayer *player = new QMediaPlayer();
//    player->setMedia(QUrl::fromLocalFile("C:/SVN/honda.mp3"));
//    player->setVolume(50);
//    player->play();

//    QSound::play("C:/SVN/0001.wav");

    qDebug(Q_FUNC_INFO);
}
