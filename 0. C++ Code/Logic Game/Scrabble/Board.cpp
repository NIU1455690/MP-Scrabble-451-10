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
	for (int i = 0; i < BOARD_COLS_AND_ROWS; i++)
	{
		for (int j = 0; j < BOARD_COLS_AND_ROWS; j++)
		{
			//	m_cells[i][j].
		}
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
