//
//  Board.cpp
//  Scrabble
//

#include "Board.h"
#include <algorithm>
#include <iostream>
#include <fstream>

Board::Board()
{
	ifstream fichero;

	fichero.open("data/Configuration/board.txt");

	if (fichero.is_open())
	{
		int fila, columna;
		ScoreEffect efecto;

		do
		{
			fichero >> fila >> columna;
			fichero >> efecto;
			
			m_cells[fila][columna].setScoreEffect(efecto);

		} while (!fichero.eof());

		fichero.close();
	}
}

Board::~Board()
{
	delete[] m_cells;
}

PositionResult Board::setTile(Tile& tile, const BoardPosition& boardPos)
{
	if (m_cells[boardPos.getCol()][boardPos.getRow()].getEmpty())
	{
		m_cells[boardPos.getCol()][boardPos.getRow()].setTile(tile);
		m_cells[boardPos.getCol()][boardPos.getRow()].setEmpty(false);
	}

	return PositionResult();
}

CurrentWordResult Board::checkCurrentWord(int& points)
{
	return CurrentWordResult();
}

void Board::sendCurrentWordToBoard()
{
}

void Board::removeCurrentWord()
{
}
