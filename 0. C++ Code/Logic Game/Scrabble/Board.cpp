//
//  Board.cpp
//  Scrabble
//

#include "Board.h"
#include <algorithm>
#include <iostream>

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

PositionResult Board::setTile(Tile& tile, const BoardPosition& boardPos)
{
	PositionResult resultado = VALID_POSITION;

	cout << "letraa" << tile.getLetter() << endl;

	int x = boardPos.getRow();
	int y = boardPos.getCol();

	if (m_cells[x][y].getEmpty())
	{
		if ((0 <= x && x < BOARD_COLS_AND_ROWS) && (0 <= y && y < BOARD_COLS_AND_ROWS))
		{
			m_cells[x][y].setTile(tile);
			m_cells[x][y].setEmpty(false);
			m_cells[x][y].setTilePlayed(true);
			m_currentWord.push_back(boardPos);
		}
		else
		{
			resultado = INVALID_POSITION;
		}
	}
	else
	{
		resultado = NOT_EMPTY;
	}

	return resultado;
}

CurrentWordResult Board::checkCurrentWord(int& points)
{
	CurrentWordResult palabraResultante = INVALID_WORD_OF_ONE_LETTER;
	bool casillaCentral = false;

	Tile t;
	t.setLetter('t');

	string palabra = "";

	if (m_currentWord.size() == 1) palabraResultante = INVALID_WORD_OF_ONE_LETTER;
	else
	{
		for (int i = 0; i < m_currentWord.size() - 1; i++)
		{
			if (m_currentWord[i].getRow() == 7 && m_currentWord[i].getCol() == 7) casillaCentral = true;
		}
		if (!casillaCentral) palabraResultante = INVALID_START_NOT_IN_CENTER;

		//m_cells[7][7].setTile(t);

		for (int i = 0; i < m_currentWord.size(); i++)
		{
			cout << "la letra es : " << m_cells[7][7].getTile().getLetter() << endl;
			palabra += m_cells[m_currentWord[i].getRow()][m_currentWord[i].getCol()].getTile().getLetter();
		}

		cout << "palabra es:" << palabra << endl;

	}

	return palabraResultante;
}

void Board::sendCurrentWordToBoard()
{
}

void Board::removeCurrentWord()
{
	int x, y;

	for (int i = m_currentWord.size() - 1; i >= 0; i--)
	{
		x = m_currentWord[i].getRow();
		y = m_currentWord[i].getCol();
		m_cells[x][y].setEmpty(true);
		m_cells[x][y].setTile(Tile());
		m_cells[x][y].setTilePlayed(false);
		m_currentWord.pop_back();
	}
}