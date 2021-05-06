//
//  Board.cpp
//  Scrabble
//

#include "Board.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

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

PositionResult Board::setTile(Tile tile, const BoardPosition& boardPos)
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
				int min, max;
				bool consecutiva = currentWordConsecutive(alineacion, min, max);
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
					else {
						if (!m_emptyBoard)
						{
							vector<BoardPosition> conexiones;
							bool conectada = currentWordConnected(alineacion, min, max, conexiones);
							if (!conectada)
							{
								palabraResultante = INVALID_NOT_CONNECTION;
							}
						}
						
					}
				}
			}
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

bool Board::currentWordConsecutive(Alignment &alineacion, int& min, int&max)
{
	int i, fila, col;
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

bool Board::currentWordConnected(Alignment& alineacion, int min, int max, vector<BoardPosition>& conexiones)
{
	bool conectada = false;
	int fila, col;

	if (alineacion == HORIZONTAL)
	{
		fila = m_currentWord[0].getRow();
		for (int i = min; i <= max; i++)
		{
			if (fila < BOARD_COLS_AND_ROWS-1) {

				if (!m_cells[fila + 1][i].getEmpty())
				{
					BoardPosition posicion;
					posicion.setRow(fila + 1);
					posicion.setCol(i);
					conexiones.push_back(posicion);

					if (!conectada)
					{
						conectada = true;
					}
				}
			}
			
			if (fila > 0) {
				if (!m_cells[fila - 1][i].getEmpty())
				{
					BoardPosition posicion;
					posicion.setRow(fila - 1);
					posicion.setCol(i);
					conexiones.push_back(posicion);

					if (!conectada)
					{
						conectada = true;
					}
				}
			}
		}

		if (max < BOARD_COLS_AND_ROWS - 1)
		{
			if (!m_cells[fila][max + 1].getEmpty())
			{
				BoardPosition posicion;
				posicion.setRow(fila);
				posicion.setCol(max + 1);
				conexiones.push_back(posicion);

				if (!conectada)
				{
					conectada = true;
				}
			}
		}

		if (min > 0) {
			if (!m_cells[fila][min - 1].getEmpty())
			{
				BoardPosition posicion;
				posicion.setRow(fila);
				posicion.setCol(min - 1);
				conexiones.push_back(posicion);

				if (!conectada)
				{
					conectada = true;
				}
			}
		}
	}
	else if (alineacion == VERTICAL)
	{
		col = m_currentWord[0].getCol();
		for (int i = min; i <= max; i++)
		{
			if (col < BOARD_COLS_AND_ROWS - 1) {

				if (!m_cells[i][col + 1].getEmpty())
				{
					BoardPosition posicion;
					posicion.setRow(i);
					posicion.setCol(col + 1);
					conexiones.push_back(posicion);

					if (!conectada)
					{
						conectada = true;
					}
				}
			}

			if (col > 0) {
				if (!m_cells[i][col - 1].getEmpty())
				{
					BoardPosition posicion;
					posicion.setRow(i);
					posicion.setCol(col - 1);
					conexiones.push_back(posicion);

					if (!conectada)
					{
						conectada = true;
					}
				}
			}
		}

		if (max < BOARD_COLS_AND_ROWS - 1)
		{
			if (!m_cells[max + 1][col].getEmpty())
			{
				BoardPosition posicion;
				posicion.setRow(max + 1);
				posicion.setCol(col);
				conexiones.push_back(posicion);

				if (!conectada)
				{
					conectada = true;
				}
			}
		}

		if (min > 0) {
			if (!m_cells[min - 1][col].getEmpty())
			{
				BoardPosition posicion;
				posicion.setRow(min - 1);
				posicion.setCol(col);
				conexiones.push_back(posicion);

				if (!conectada)
				{
					conectada = true;
				}
			}
		}
	}

	return conectada;
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

	m_currentWord.clear();

	if (m_emptyBoard)
	{
		m_emptyBoard = false;
	}
}

void Board::removeCurrentWord()
{
	int fila, col;

	for (int i = 0; i < m_currentWord.size(); i++)
	{
		Tile tile;
		fila = m_currentWord[i].getRow();
		col = m_currentWord[i].getCol();
		m_cells[fila][col].setEmpty(true);
		m_cells[fila][col].setTile(tile);
		m_cells[fila][col].setTilePlayed(false);
	}

	m_currentWord.clear();
}