//
//  Board.cpp
//  Scrabble
//

#include "Board.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>

Board::Board()
{
	ifstream fichero;
	string nombreFichero = "../../1. Resources/data/Configuration/board.txt";
	fichero.open(nombreFichero);
	
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

	m_emptyBoard = true;
	m_dictionary.setLanguage(ENGLISH);
}

PositionResult Board::setTile(Tile& tile, const BoardPosition& boardPos)
{
	PositionResult resultado = VALID_POSITION;

	cout << "letra: " << tile.getLetter() << endl;

	int x = boardPos.getRow();
	int y = boardPos.getCol();

	if (m_cells[x][y].getEmpty())
	{
		if ((0 <= x && x < BOARD_COLS_AND_ROWS) && (0 <= y && y < BOARD_COLS_AND_ROWS))
		{
			m_cells[x][y].setTile(tile);
			m_cells[x][y].setEmpty(false);
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
	CurrentWordResult palabraResultante = ALL_CORRECT;

	string palabra = "";

	if (m_currentWord.size() == 1) palabraResultante = INVALID_WORD_OF_ONE_LETTER;
	else
	{
		// Primera jugada
		bool casillaCentral = false;

		if (m_emptyBoard)
		{
			int i = 0;
			while (!casillaCentral && i < m_currentWord.size())
			{
				if (m_currentWord[i].getRow() == 7 && m_currentWord[i].getCol() == 7) casillaCentral = true;
				else i++;
			}

			if (!casillaCentral) palabraResultante = INVALID_START_NOT_IN_CENTER;
		}

		if (!m_emptyBoard || casillaCentral) {

			Alignment alineacion = currentWordAlignment();

			if (alineacion == NOT_ALIGNED)
			{
				palabraResultante = INVALID_NOT_ALIGNED;
			}
			else
			{
				bool consecutiva = currentWordConsecutive(alineacion);
				if (!consecutiva)
				{
					palabraResultante = INVALID_NOT_ALIGNED;
				}
				else
				{
					int fila, col;
					for (int i = 0; i < m_currentWord.size(); i++)
					{
						fila = m_currentWord[i].getRow();
						col = m_currentWord[i].getCol();
						palabra += m_cells[fila][col].getTile().getLetter();
					}

					if (!m_dictionary.check(palabra))
					{
						palabraResultante = INVALID_WORDS_NOT_IN_DICTIONARY;
					}
				}
			}
			/*
			//m_cells[7][7].setTile(t);

			for (int i = 0; i < m_currentWord.size(); i++)
			{
				cout << "la letra es : " << m_cells[7][7].getTile().getLetter() << endl;
				palabra += m_cells[m_currentWord[i].getRow()][m_currentWord[i].getCol()].getTile().getLetter();
			}

			cout << "alineacion: " << alineacion;
			cout << "palabra es:" << palabra << endl;
			*/
		}

	}

	return palabraResultante;
}

Alignment Board::currentWordAlignment()
{
	Alignment alineacion = NOT_ALIGNED;
	bool alineada = true;
	bool alineadaHorizontal = true;
	bool alineadaVertical = true;
	int ultimaFila = m_currentWord[0].getRow();
	int ultimaCol = m_currentWord[0].getCol();
	int i = 1;
	int fila, col;
	
	while (alineada && (alineadaHorizontal || alineadaVertical) && i < m_currentWord.size())
	{	
		int fila = m_currentWord[i].getRow();
		int col = m_currentWord[i].getCol();
		if (ultimaFila != fila && ultimaCol != col)
		{
			alineada = false;
		}
		else
		{
			if (ultimaFila != fila)
			{
				alineadaHorizontal = false;
			}

			if (ultimaCol != col)
			{
				alineadaVertical = false;
			}

			ultimaFila = fila;
			ultimaCol = col;
			i++;
		}
	}

	if (alineadaHorizontal)
	{
		alineacion = HORIZONTAL;
	}
	else if (alineadaVertical)
	{
		alineacion = VERTICAL;
	}

	return alineacion;
}

bool Board::currentWordConsecutive(Alignment &alineacion)
{
	int min, max, i, fila, col;
	bool consecutiva = true;

	if (alineacion == HORIZONTAL)
	{
		min = m_currentWord[0].getCol();
		max = m_currentWord[0].getCol();
		fila = m_currentWord[0].getRow();

		for (i = 0; i < m_currentWord.size(); i++)
		{
			if (m_currentWord[i].getCol() < min)
			{
				min = m_currentWord[i].getCol();
			}
			else if (m_currentWord[i].getCol() > max)
			{
				max = m_currentWord[i].getCol();
			}
		}

		i = 1;

		while (consecutiva && min + i < max)
		{	
			if (m_cells[fila][min + i].getEmpty())
			{
				consecutiva = false;
			}
			else
			{
				i++;
			}
		}
	}
	else if (alineacion == VERTICAL)
	{
		min = m_currentWord[0].getRow();
		max = m_currentWord[0].getRow(); 
		col = m_currentWord[0].getCol();

		for (i = 0; i < m_currentWord.size(); i++)
		{
			if (m_currentWord[i].getRow() < min)
			{
				min = m_currentWord[i].getRow();
			}
			else if (m_currentWord[i].getRow() > max)
			{
				max = m_currentWord[i].getRow();
			}
		}

		i = 1;

		while (consecutiva && min + i < max)
		{
			
			if (m_cells[min + i][col].getEmpty())
			{
				consecutiva = false;
			}
			else
			{
				i++;
			}
		}
	}
	
	return consecutiva;
}

void Board::sendCurrentWordToBoard()
{
	int fila, col;

	for (int i = 0; i < m_currentWord.size(); i++)
	{
		fila = m_currentWord[i].getRow();
		col = m_currentWord[i].getCol();
		m_cells[fila][col].setTilePlayed(true);
	}

	removeCurrentWord();

	if (m_emptyBoard)
	{
		m_emptyBoard = false;
	}
}

void Board::removeCurrentWord()
{
	int x, y;

	for (int i = m_currentWord.size() - 1; i >= 0; i--)
	{
		Tile tile;
		x = m_currentWord[i].getRow();
		y = m_currentWord[i].getCol();
		m_cells[x][y].setEmpty(true);
		m_cells[x][y].setTile(tile);
		m_cells[x][y].setTilePlayed(false);
		m_currentWord.pop_back();
	}
}