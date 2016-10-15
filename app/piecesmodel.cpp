#include "piecesmodel.h"
#include "puzzlepiece.h"

#include <QIcon>
#include <QMimeData>

PiecesModel::PiecesModel(int pieceCountBySide,
                         int pieceSize,
                         QObject *parent)
    : QAbstractListModel(parent),
      _pieceCountBySide(pieceCountBySide),
      _pieceSize(pieceSize)
{
}

QVariant PiecesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        QPixmap currentPixmap = _pixmaps.value(index.row());
        QPixmap scaledPixmap = currentPixmap.scaled(_pieceSize, _pieceSize,
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation);
        return QIcon(scaledPixmap);
    }

    if (role == Qt::UserRole)
        return _pixmaps.value(index.row());

    if (role == Qt::UserRole + 1)
        return _locations.value(index.row());

    return QVariant();
}

void PiecesModel::addPiece(const QPixmap &pixmap, const QPoint &location)
{
    int row = _pixmaps.size();
    if (int(2.0 * qrand() / (RAND_MAX + 1.0)) == 1)
        row = 0;

    beginInsertRows(QModelIndex(), row, row);
    _pixmaps.insert(row, pixmap);
    _locations.insert(row, location);
    endInsertRows();
}

Qt::ItemFlags PiecesModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return (QAbstractListModel::flags(index)|Qt::ItemIsDragEnabled);

    return Qt::ItemIsDropEnabled;
}

bool PiecesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    if (row >= _pixmaps.size() || row + count <= 0)
        return false;

    int beginRow = qMax(0, row);
    int endRow = qMin(row + count - 1, _pixmaps.size() - 1);

    beginRemoveRows(parent, beginRow, endRow);

    while (beginRow <= endRow)
    {
        _pixmaps.removeAt(beginRow);
        _locations.removeAt(beginRow);
        ++beginRow;
    }

    endRemoveRows();
    return true;
}

QStringList PiecesModel::mimeTypes() const
{
    QStringList types;
    types << PuzzlePiece::puzzlePieceMimeStr();
    return types;
}

QMimeData *PiecesModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            QPixmap pixmap = qvariant_cast<QPixmap>(data(index, Qt::UserRole));
            QPoint location = data(index, Qt::UserRole + 1).toPoint();
            stream << pixmap << location;
        }
    }

    QString mimeType = PuzzlePiece::puzzlePieceMimeStr();
    mimeData->setData(mimeType, encodedData);
    return mimeData;
}

bool PiecesModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
    QString mimeType = PuzzlePiece::puzzlePieceMimeStr();
    if (!data->hasFormat(mimeType))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int endRow;

    if (!parent.isValid())
    {
        if (row < 0)
            endRow = _pixmaps.size();
        else
            endRow = qMin(row, _pixmaps.size());
    }
    else
    {
        endRow = parent.row();
    }

    QByteArray encodedData = data->data(mimeType);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd())
    {
        QPixmap pixmap;
        QPoint location;
        stream >> pixmap >> location;

        beginInsertRows(QModelIndex(), endRow, endRow);
        _pixmaps.insert(endRow, pixmap);
        _locations.insert(endRow, location);
        endInsertRows();

        ++endRow;
    }

    return true;
}

int PiecesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return _pixmaps.size();
}

Qt::DropActions PiecesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void PiecesModel::addPieces(const QPixmap& pixmap)
{
    int firstRow = 0;
    int lastRow = (_pieceCountBySide * _pieceCountBySide);
    beginRemoveRows(QModelIndex(), firstRow, lastRow);
    _pixmaps.clear();
    _locations.clear();
    endRemoveRows();

    for (int y = 0; y < _pieceCountBySide; ++y)
    {
        for (int x = 0; x < _pieceCountBySide; ++x)
        {
            QRect pieceRect(x * _pieceSize,
                            y * _pieceSize,
                            _pieceSize, _pieceSize);
            QPixmap pieceImage = pixmap.copy(pieceRect);
            addPiece(pieceImage, QPoint(x, y));
        }
    }
}
