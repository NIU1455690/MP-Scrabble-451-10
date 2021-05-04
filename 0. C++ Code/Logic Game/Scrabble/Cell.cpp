//
//  Cell.cpp
//  Scrabble
//

#include "Cell.h"

using namespace std;

ifstream& operator>>(ifstream& input, ScoreEffect& s)
{
    string efecto;

    input >> efecto;

    if (efecto == "DL")
    {
        s = DL;
    }
    else if (efecto == "TL")
    {
        s = TL;
    }
    else if (efecto == "DW")
    {
        s = DW;
    }
    else if (efecto == "TW")
    {
        s = TW;
    }
    else if (efecto == "NO_EFFECT")
    {
        s = NO_EFFECT;
    }
    return input;
}