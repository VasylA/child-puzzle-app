#include "puzzlewidget.h"
#include "puzzlepiece.h"

#include <QtWidgets>

PuzzleWidget::PuzzleWidget(int piecesBySide, int imageSize, QWidget *parent)
    : QWidget(parent),
      _piecesBySide(piecesBySide),
      _imageSize(imageSize),
      _inPlace(0)
{
    setAcceptDrops(true);
    setMinimumSize(_imageSize, _imageSize);
    setMaximumSize(_imageSize, _imageSize);
}

void PuzzleWidget::clear()
{
    _pieceLocations.clear();
    _piecePixmaps.clear();
    _pieceRects.clear();
    _highlightedRect = QRect();
    _inPlace = 0;
    update();
}

void PuzzleWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QString mimeType = PuzzlePiece::puzzlePieceMimeStr();
    if (event->mimeData()->hasFormat(mimeType))
        event->accept();
    else
        event->ignore();
}

void PuzzleWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = _highlightedRect;
    _highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

void PuzzleWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect updateRect = _highlightedRect.united(targetSquare(event->pos()));

    QRect targetRect = targetSquare(event->pos());
    QString mimeType = PuzzlePiece::puzzlePieceMimeStr();    
    if (event->mimeData()->hasFormat(mimeType)
            && (findPiece(targetRect) == UNDEFINED_PIECE_INDEX))
    {
        _highlightedRect = targetRect;
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        _highlightedRect = QRect();
        event->ignore();
    }

    update(updateRect);
}

void PuzzleWidget::dropEvent(QDropEvent *event)
{
    QString mimeType = PuzzlePiece::puzzlePieceMimeStr();
    if (event->mimeData()->hasFormat(mimeType)
            && findPiece(targetSquare(event->pos())) == UNDEFINED_PIECE_INDEX)
    {
        QString mimeType = PuzzlePiece::puzzlePieceMimeStr();
        QByteArray pieceData = event->mimeData()->data(mimeType);
        QDataStream stream(&pieceData, QIODevice::ReadOnly);
        QRect square = targetSquare(event->pos());
        QPixmap pixmap;
        QPoint location;
        stream >> pixmap >> location;

        _pieceLocations.append(location);
        _piecePixmaps.append(pixmap);
        _pieceRects.append(square);

        _highlightedRect = QRect();
        update(square);

        event->setDropAction(Qt::MoveAction);
        event->accept();

        if (location == QPoint(square.x()/pieceSize(), square.y()/pieceSize()))
        {
            _inPlace++;
            const int maxPartsCount = (_piecesBySide * _piecesBySide);
            if (_inPlace == maxPartsCount)
                emit puzzleCompleted();
        }
    }
    else
    {
        _highlightedRect = QRect();
        event->ignore();
    }
}

int PuzzleWidget::findPiece(const QRect &pieceRect) const
{
    for (int i = 0; i < _pieceRects.size(); ++i)
    {
        if (pieceRect == _pieceRects[i])
            return i;
    }
    return -1;
}

void PuzzleWidget::mousePressEvent(QMouseEvent *event)
{
    QRect square = targetSquare(event->pos());
    int pieceIndex = findPiece(square);

    if (pieceIndex == UNDEFINED_PIECE_INDEX)
        return;

    QPoint location = _pieceLocations[pieceIndex];
    QPixmap pixmap = _piecePixmaps[pieceIndex];
    _pieceLocations.removeAt(pieceIndex);
    _piecePixmaps.removeAt(pieceIndex);
    _pieceRects.removeAt(pieceIndex);

    if (location == QPoint(square.x()/pieceSize(), square.y()/pieceSize()))
        _inPlace--;

    update(square);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << pixmap << location;

    QMimeData *mimeData = new QMimeData;
    QString mimeType = PuzzlePiece::puzzlePieceMimeStr();
    mimeData->setData(mimeType, itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - square.topLeft());
    drag->setPixmap(pixmap);

    if (drag->start(Qt::MoveAction) == 0)
    {
        _pieceLocations.insert(pieceIndex, location);
        _piecePixmaps.insert(pieceIndex, pixmap);
        _pieceRects.insert(pieceIndex, square);
        update(targetSquare(event->pos()));

        if (location == QPoint(square.x()/pieceSize(), square.y()/pieceSize()))
            _inPlace++;
    }
}

void PuzzleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(), Qt::white);

    if (_highlightedRect.isValid())
    {
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(_highlightedRect.adjusted(0, 0, -1, -1));
    }

    for (int i = 0; i < _pieceRects.size(); ++i)
        painter.drawPixmap(_pieceRects[i], _piecePixmaps[i]);

    painter.end();
}

const QRect PuzzleWidget::targetSquare(const QPoint &position) const
{
    return QRect(position.x()/pieceSize() * pieceSize(),
                 position.y()/pieceSize() * pieceSize(),
                 pieceSize(),
                 pieceSize());
}

int PuzzleWidget::pieceSize() const
{
    return _imageSize / _piecesBySide;
}

int PuzzleWidget::imageSize() const
{
    return _imageSize;
}
