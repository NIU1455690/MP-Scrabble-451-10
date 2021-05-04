//
//  Cell.hpp
//  Scrabble
//


#ifndef Cell_hpp
#define Cell_hpp

#include <stdio.h>
#include "Tile.h"

enum class ScoreEffect {
    DL = 0,
    TL,
    DW,
    TW,
    NO_EFFECT
};

class Cell
{
public:
    ScoreEffect getScoreEffect() const { return m_scoreEffect; }
    Tile getTile() const { return m_tile; }
    bool getEmpty() const { return m_empty; }
    bool getTilePlayed() const { return m_tilePlayed; }
    void setScoreEffect(ScoreEffect efectoPuntuacion) { m_scoreEffect = efectoPuntuacion; }
    void setTile(Tile casilla) {
        m_tile.setLetter(casilla.getLetter());
        m_tile.setScore(casilla.getScore());
    }
    void setEmpty(bool vacio) { m_empty = vacio; }
    void setTilePlayed(bool fichaJugada) { m_tilePlayed = fichaJugada; }
private:
    ScoreEffect m_scoreEffect;
    Tile m_tile;
    bool m_empty;
    bool m_tilePlayed;
};


#endif /* Cell_hpp */
