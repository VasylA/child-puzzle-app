#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QString>

class PuzzlePiece
{
public:
    static QString puzzlePieceMimeStr()
    {
        return "image/x-puzzle-piece";
    }
};


#endif // PUZZLEPIECE_H
