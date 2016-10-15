
#include "mainwindow.h"
#include "piecesmodel.h"
#include "puzzlewidget.h"

#include <QtWidgets>
#include <QApplication>

//#include <QtMultimedia/QMediaPlayer>
//#include <QSound>

#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _piecesList(nullptr),
      _puzzleWidget(nullptr),
      _model(nullptr)
{
    setupWidgets();
    _model = new PiecesModel(PIECE_COUNT_BY_SIDE, _puzzleWidget->pieceSize(), this);
    _piecesList->setModel(_model);

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
    playMusic();
}

void MainWindow::updateLcdTime()
{
    _remainingTimeWidget->display(_puzzleTimer.remainingTime() / 1000);
    QTimer::singleShot(1000, this, &MainWindow::updateLcdTime);
}

void MainWindow::setupTimer()
{
    _puzzleTimer.setInterval(TIMER_PERIOD);
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

    int imageSize = _puzzleWidget->imageSize();
    _puzzleImage = currentPixmap.scaled(imageSize, imageSize,
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);

    qsrand(QCursor::pos().x() ^ QCursor::pos().y());

    _model->addPieces(_puzzleImage);
    _puzzleWidget->clear();
}

void MainWindow::gameOver()
{
    _puzzleTimer.stop();

    QMessageBox::warning(nullptr, tr("GAME OVER"),
                         tr("You lose the game."),
                         QMessageBox::Ok);


    QTimer::singleShot(TIMER_PERIOD, this, &MainWindow::resetPuzzle);
}

void MainWindow::resetPuzzle()
{
    setupPuzzle();

    _puzzleTimer.stop();
    _puzzleTimer.start(TIMER_PERIOD);

    updateLcdTime();
}

void MainWindow::setupWidgets()
{
    const int MARGINS = 20;
    QSize availableScreenSize = qApp->primaryScreen()->availableSize();
    availableScreenSize.rheight() -= 2 * MARGINS;

    int imageHeight = availableScreenSize.height();

    _puzzleWidget = new PuzzleWidget(PIECE_COUNT_BY_SIDE, imageHeight);

    static const int offset = 20;
    int pieceSize = _puzzleWidget->pieceSize();
    int iconSize = pieceSize - offset;
    int gridSize = pieceSize;

    _piecesList = new QListView;
    _piecesList->setDragEnabled(true);
    _piecesList->setViewMode(QListView::IconMode);

//    _piecesList->setFixedWidth(pieceSize + 2 * offset);
    _piecesList->setIconSize(QSize(iconSize, iconSize));
    _piecesList->setGridSize(QSize(gridSize, gridSize));
    _piecesList->setSpacing(10);

    _piecesList->setMovement(QListView::Snap);
    _piecesList->setAcceptDrops(true);
    _piecesList->setDropIndicatorShown(true);

    PiecesModel *model = new PiecesModel(PIECE_COUNT_BY_SIDE,
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

    QFrame *gameFrame = new QFrame;
    QHBoxLayout *gameLayout = new QHBoxLayout(gameFrame);
    gameLayout->addWidget(_piecesList);
    gameLayout->addLayout(rightLayout);

//    QFrame *centralFrame = new QFrame;
//    QHBoxLayout *centralLayout = new QHBoxLayout(centralFrame);
//    centralLayout->addWidget(gameFrame);
    setCentralWidget(gameFrame);
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
