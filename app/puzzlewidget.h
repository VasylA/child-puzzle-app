#ifndef PUZZLEWIDGET_H
#define PUZZLEWIDGET_H

#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QString;
QT_END_NAMESPACE

class PuzzleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PuzzleWidget(int rowCount, int columnCount, QSize imageSize, QWidget *parent = 0);
    void clear();

    QSize pieceSize() const;
    QSize imageSize() const;

    void setBackgroundImage(const QString &imagePath);

signals:
    void puzzleCompleted();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position) const;

    static const int UNDEFINED_PIECE_INDEX = -1;

    QList<QRect> _pieceRects;
    QList<QPoint> _pieceLocations;
    QList<QPixmap> _piecePixmaps;
    QRect _highlightedRect;

    QString _backgroundImagePath;

    int _piecesInRow;
    int _piecesInColumn;

    QSize _imageSize;

    int _inPlace;
};

#endif // PUZZLEWIDGET_H
