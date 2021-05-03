//
//  Tile.hpp
//  Scrabble
//

#ifndef Tile_hpp
#define Tile_hpp


class Tile
{
public:
    char getLetter() const { return m_letter; }
    int getScore() const { return m_score; }
    void setLetter(char letra) { m_letter = letra; }
    void setScore(unsigned int puntuacion) { m_score = puntuacion; }

private:
    char m_letter;
    unsigned int m_score;
};


#endif /* Tile_hpp */
