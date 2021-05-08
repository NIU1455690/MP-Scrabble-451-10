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
			fichero >> columna >> fila;
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

	cout << "===> setTile(): " << tile.getLetter() << endl;

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
	points = 0;
	string palabra = "";

	if (m_currentWord.size() == 1) // 1 letra -> paramos
	{
		palabraResultante = INVALID_WORD_OF_ONE_LETTER;
	}
	else // Más de 1 letra -> seguimos
	{
		bool casillaCentral = false;

		if (m_emptyBoard) // Primera jugada -> miramos casilla central
		{
			int i = 0;
			while (!casillaCentral && i < m_currentWord.size())
			{
				if (m_currentWord[i].getRow() == 7 && m_currentWord[i].getCol() == 7) casillaCentral = true;
				else i++;
			}

			if (!casillaCentral) palabraResultante = INVALID_START_NOT_IN_CENTER;
		}

		if (!m_emptyBoard || casillaCentral) { // Si no es la primera jugada o ha pasado el test de casilla central, seguimos

			Alignment alineacion = currentWordAlignment();

			if (alineacion == NOT_ALIGNED) // No alineada -> paramos
			{
				palabraResultante = INVALID_NOT_ALIGNED;
			}
			else // Alineada -> seguimos
			{
				int min, max;
				bool interseccion = false;
				bool consecutiva = currentWordConsecutive(alineacion, min, max, interseccion);

				if (!consecutiva) // No consecutiva -> paramos
				{
					palabraResultante = INVALID_NOT_ALIGNED;
				}
				else // Consecutiva -> seguimos
				{
					if (!m_emptyBoard) // Si ya hay palabras en el tablero, mirar conexiones
					{
						vector<BoardPosition> conexiones;
						bool conectada = currentWordConnected(alineacion, min, max, conexiones, interseccion);
						if (!conectada) // Si no está conectada, no seguimos
						{
							palabraResultante = INVALID_NOT_CONNECTION;
						}
						else // Si hay conexión, seguimos
						{
							newWords(alineacion, min, max, conexiones); // Guardamos todas las combinaciones creadas
						}
					}
				}
			}
		}

		if (palabraResultante == ALL_CORRECT) // Más de una letra, alineada y consecutiva -> comprovamos diccionario
		{
			bool valid = true;
			palabra = readWordFromVector(m_currentWord);

			if (!m_dictionary.check(palabra))
			{
				valid = false;
			} 
			else if (!m_emptyBoard) // Si palabra actual correcta -> Miramos palabras conectadas si no es primera jugada
			{
				int i = 0;
				while (valid && i < m_connectedWords.size()) 
				{
					palabra = readWordFromVector(m_connectedWords[i]);
					if (!m_dictionary.check(palabra))
					{
						valid = false;
					}
					else {
						i++;
					}
				}
			}

			if (!valid) // Si alguna palabra no está en el diccionario: error
			{
				palabraResultante = INVALID_WORDS_NOT_IN_DICTIONARY;
			}
			else // Todo correcto -> calculamos puntos
			{
				points = pointsWords();
			}
		}

	}

	return palabraResultante;
}

string Board::readWordFromVector(vector<BoardPosition>& vector)
{
	string palabra = "";

	int fila, col;
	for (int i = 0; i < vector.size(); i++)
	{
		fila = vector[i].getRow();
		col = vector[i].getCol();
		palabra += m_cells[fila][col].getTile().getLetter();
	}

	return palabra;
}


void Board::newWords(Alignment& alineacion, int& min, int& max, vector<BoardPosition>& conexiones)
{
	int fila, col;
	bool buscar = true;

	//=====================HORIZONTAL======================================================
	if (alineacion == HORIZONTAL)
	{
		// RECORREMOS TODAS LAS CONEXIONES
		for (int i = 0; i < conexiones.size(); i++)
		{
			fila = m_currentWord[0].getRow(); // Horizontal -> fila constante
			buscar = true;

			// CONEXION HORIZONTAL con la palabra-> actualizarla
			if (conexiones[i].getRow() == fila)
			{
				col = min;
				while (col - 1 >= 0 && buscar) // Mientras haya fichas o no sea el final, buscamos izquierda
				{
					if (!m_cells[fila][col - 1].getEmpty())
					{
						col--;
						BoardPosition bp = BoardPosition(col, fila);
						m_currentWord.insert(m_currentWord.begin(), bp); // Insertar por inicio
					}
					else
					{
						buscar = false;
					}
				}
				min = col; // Actualizamos el minimo

				col = max;
				buscar = true;
				while (col + 1 < BOARD_COLS_AND_ROWS && buscar) // Mientras haya fichas o no sea el final, buscamos derecha
				{
					if (!m_cells[fila][col + 1].getEmpty())
					{
						col++;
						BoardPosition bp = BoardPosition(col, fila);
						m_currentWord.push_back(bp); // Insertar al final
					}
					else
					{
						buscar = false;
					}
				}
				max = col; // Actualizamos el maximo
			}
			// CONEXION VERTICAL -> Nueva palabra, buscaremos sus min y max 
			else
			{
				fila = conexiones[i].getRow(); 
				col = conexiones[i].getCol(); 
				int newWordMin = fila; 
				int newWordMax = fila;
				
				while (fila - 1 >= 0 && buscar) // Mientras haya fichas o no sea el final, buscamos minimo arriba
				{
					if (!m_cells[fila - 1][col].getEmpty())
					{
						fila--;
						newWordMin = fila;
					}
					else
					{
						buscar = false;
					}
				}

				buscar = true;
				while (fila + 1 < BOARD_COLS_AND_ROWS && buscar) // Mientras haya fichas o no sea el final, buscamos máximo abajo
				{
					if (!m_cells[fila + 1][col].getEmpty())
					{
						fila++;
						newWordMax = fila;
					}
					else
					{
						buscar = false;
					}
				}

				// Recorrer min y max para tener palabra nueva
				vector <BoardPosition> newWord;
				for (int i = newWordMin; i <= newWordMax; i++)
				{
					BoardPosition bp = BoardPosition(col, i);
					newWord.push_back(bp);
				}

				m_connectedWords.push_back(newWord); // Añadir nuestra palabra al vector de palabras
			}
		}
	}
	//=====================VERTICAL======================================================
	else if (alineacion == VERTICAL)
	{
		// RECORREMOS TODAS LAS CONEXIONES
		for (int i = 0; i < conexiones.size(); i++)
		{
			col = m_currentWord[0].getCol(); // Vertical -> columna constante
			buscar = true;

			// CONEXION VERTICAL con la palabra-> actualizarla
			if (conexiones[i].getCol() == col)
			{
				fila = min;
				while (fila - 1 >= 0 && buscar) // Mientras haya fichas o no sea el final, buscamos arriba
				{
					if (!m_cells[fila - 1][col].getEmpty())
					{
						fila--;
						BoardPosition bp = BoardPosition(col, fila);
						m_currentWord.insert(m_currentWord.begin(), bp); // Insertar por inicio
					}
					else
					{
						buscar = false;
					}
				}
				min = fila; // Actualizamos el minimo

				fila = max;
				buscar = true;
				while (fila + 1 < BOARD_COLS_AND_ROWS && buscar) // Mientras haya fichas o no sea el final, buscamos abajo
				{
					if (!m_cells[fila + 1][col].getEmpty())
					{
						fila++;
						BoardPosition bp = BoardPosition(col, fila);
						m_currentWord.push_back(bp); // Insertar al final
					}
					else
					{
						buscar = false;
					}
				}
				max = fila; // Actualizamos el maximo
			}
			// CONEXION HORIZONTAL -> Nueva palabra, buscaremos sus min y max 
			else
			{
				fila = conexiones[i].getRow();
				col = conexiones[i].getCol();
				int newWordMin = col;
				int newWordMax = col;

				
				while (col - 1 >= 0 && buscar) // Mientras haya fichas o no sea el final, buscamos minimo izquierda
				{
					if (!m_cells[fila][col - 1].getEmpty())
					{
						col--;
						newWordMin = col;
					}
					else
					{
						buscar = false;
					}
				}

				buscar = true;
				while (col + 1 < BOARD_COLS_AND_ROWS && buscar) // Mientras haya fichas o no sea el final, buscamos máximo derecha
				{
					if (!m_cells[fila][col + 1].getEmpty())
					{
						col++;
						newWordMax = col;
					}
					else
					{
						buscar = false;
					}
				}

				// Recorrer min y max para tener palabra nueva
				vector <BoardPosition> newWord;
				for (int i = newWordMin; i <= newWordMax; i++)
				{
					BoardPosition bp = BoardPosition(i, fila);
					newWord.push_back(bp);
				}

				m_connectedWords.push_back(newWord); // Añadir nuestra palabra al vector de palabras
			}
		}
	}
}

int Board::pointsWords()
{
	int puntosTotales = 0;

	for (int i = 0; i < m_connectedWords.size(); i++)
	{
		puntosTotales += pointsWord(m_connectedWords[i]);
	}

	puntosTotales += pointsWord(m_currentWord);

	return puntosTotales;
}

int Board::pointsWord(vector<BoardPosition>& word)
{
	int fila, col;
	int puntosPalabra = 0;
	int puntosFicha = 0;
	int doblePalabra = 1;
	int triplePalabra = 1;
	Cell celda;
	Tile ficha;

	for (int i = 0; i < word.size(); i++)
	{
		fila = word[i].getRow();
		col = word[i].getCol();
		celda = m_cells[fila][col];
		ficha = celda.getTile();
		puntosFicha = ficha.getScore();

		if (!celda.getTilePlayed())
		{
			switch (celda.getScoreEffect())
			{
			case DL:
				puntosPalabra += (puntosFicha * 2);
				break;
			case TL:
				puntosPalabra += (puntosFicha * 3);
				break;
			case DW:
				puntosPalabra += puntosFicha;
				doblePalabra *= 2;
				break;
			case TW:
				puntosPalabra += puntosFicha;
				triplePalabra *= 3;
				break;
			case NO_EFFECT:
				puntosPalabra += puntosFicha;
				break;
			default:
				break;
			}
		}
		else
		{
			puntosPalabra += puntosFicha;
		}
	}

	puntosPalabra *= doblePalabra *= triplePalabra;

	return puntosPalabra;
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
		fila = m_currentWord[i].getRow();
		col = m_currentWord[i].getCol();

		if (ultimaFila != fila && ultimaCol != col) // No coincide fila ni col -> no alineada
		{
			alineada = false;
		}
		else // Coincide alguna
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

bool Board::currentWordConsecutive(Alignment &alineacion, int& min, int&max, bool& interseccion)
{
	int i, fila, col;
	bool consecutiva = true;

	if (alineacion == HORIZONTAL)
	{
		fila = m_currentWord[0].getRow(); // Horizontal -> fila constante
		min = m_currentWord[0].getCol(); // Min y max -> columna
		max = m_currentWord[0].getCol();

		for (i = 0; i < m_currentWord.size(); i++) // Buscamos min y max
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

		while (consecutiva && min + i < max) // Recorremos tablero de min a max para comprovar consecucion
		{	
			if (m_cells[fila][min + i].getEmpty())
			{
				consecutiva = false;
			}
			else
			{
				if (m_cells[fila][min + i].getTilePlayed()) // Si hay una ficha vieja, se añade a la currentWord
				{
					BoardPosition bp = BoardPosition(min + i, fila);
					m_currentWord.insert(m_currentWord.begin() + i, bp);
					if (!interseccion)
					{
						interseccion = true;
					}
				}
				i++;
			}
		}
	}
	else if (alineacion == VERTICAL)
	{	
		col = m_currentWord[0].getCol(); // Vertical -> columna constante
		min = m_currentWord[0].getRow(); // Min y max -> fila
		max = m_currentWord[0].getRow();

		for (i = 0; i < m_currentWord.size(); i++) // Buscamos min y max
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

		while (consecutiva && min + i < max) // Recorremos min y max para comprovar consecucion
		{
			if (m_cells[min + i][col].getEmpty())
			{
				consecutiva = false;
			}
			else
			{
				if (m_cells[min + i][col].getTilePlayed()) // Si hay una ficha vieja, se añade a la currentWord
				{
					BoardPosition bp = BoardPosition(col, min + i);
					m_currentWord.insert(m_currentWord.begin() + i, bp);
					if (!interseccion)
					{
						interseccion = true;
					}
				}
				i++;
			}
		}
	}
	
	return consecutiva;
}

bool Board::currentWordConnected(Alignment& alineacion, int min, int max, vector<BoardPosition>& conexiones, bool conectada)
{
	// No guardaremos las conexiones redundantes (si hay conexión a un lado de la palabra, nos da igual el otro)
	// Esto es porque si hay conexión en una dirección, tanto arriba como abajo, comprobaremos de
	// nuevo el nuevo mínimo y el nuevo máximo en esa dirección igualmente.
	bool direccionEncontrada = false;
	int fila, col;

	if (!conectada)
	{
		if (alineacion == HORIZONTAL)
		{
			fila = m_currentWord[0].getRow(); // Horizontal -> fila constante
			for (int i = min; i <= max; i++) // Recorremos todas las letras de la palabra (columnas)
			{
				if (!m_cells[fila][i].getTilePlayed()) // Comprovamos conexiones si no es una ficha ya jugada
				{
					if (fila > 0) // Miramos si hay conexion arriba siempre que esté en los límites
					{
						if (!m_cells[fila - 1][i].getEmpty())
						{
							BoardPosition bp = BoardPosition(i, fila - 1);
							conexiones.push_back(bp);
							direccionEncontrada = true;

							if (!conectada)
							{
								conectada = true;
							}
						}
					}

					if (fila < BOARD_COLS_AND_ROWS - 1 && !direccionEncontrada) // Si hay conexion abajo si está en los límites y no hay arriba
					{
						if (!m_cells[fila + 1][i].getEmpty())
						{
							BoardPosition bp = BoardPosition(i, fila + 1);
							conexiones.push_back(bp);

							if (!conectada)
							{
								conectada = true;
							}
						}
					}
				}
			}

			direccionEncontrada = false;

			if (min > 0) // Miramos si hay conexion en la misma direccion a la izquierda siempre que esté en los límites
			{
				if (!m_cells[fila][min - 1].getEmpty())
				{
					BoardPosition bp = BoardPosition(min - 1, fila);
					conexiones.push_back(bp);
					direccionEncontrada = true;

					if (!conectada)
					{
						conectada = true;
					}
				}
			}

			if (max < BOARD_COLS_AND_ROWS - 1 && !direccionEncontrada) // Miramos misma direccion derecha siempre que esté en límites y no haya a la izquierda
			{
				if (!m_cells[fila][max + 1].getEmpty())
				{
					BoardPosition bp = BoardPosition(max + 1, fila);
					conexiones.push_back(bp);

					if (!conectada)
					{
						conectada = true;
					}
				}
			}
		}
		else if (alineacion == VERTICAL)
		{
			col = m_currentWord[0].getCol(); // Vertical -> fila constante

			for (int i = min; i <= max; i++) // Recorremos todas las letras de la palabra (filas)
			{
				if (!m_cells[i][col].getTilePlayed()) // Comprovamos conexiones si no es una ficha ya jugada
				{
					if (col > 0) // Miramos a la izquierda siempre que estemos en el límite
					{
						if (!m_cells[i][col - 1].getEmpty())
						{
							BoardPosition bp = BoardPosition(col - 1, i);
							conexiones.push_back(bp);
							direccionEncontrada = true;

							if (!conectada)
							{
								conectada = true;
							}
						}
					}

					if (col < BOARD_COLS_AND_ROWS - 1 && !direccionEncontrada) { // Miramos derecha mientras dentro límite y no haya en izquierda

						if (!m_cells[i][col + 1].getEmpty())
						{
							BoardPosition bp = BoardPosition(col + 1, i);
							conexiones.push_back(bp);

							if (!conectada)
							{
								conectada = true;
							}
						}
					}
				}
			}

			direccionEncontrada = false;

			if (min > 0) // Miramos en la misma direccion arriba mientras estemos en los límites
			{
				if (!m_cells[min - 1][col].getEmpty())
				{
					BoardPosition bp = BoardPosition(col, min - 1);
					conexiones.push_back(bp);
					direccionEncontrada = true;

					if (!conectada)
					{
						conectada = true;
					}
				}
			}

			if (max < BOARD_COLS_AND_ROWS - 1 && !direccionEncontrada) // Misma dirección abajo mientras dentro límite y no haya arriba
			{
				if (!m_cells[max + 1][col].getEmpty())
				{
					BoardPosition bp = BoardPosition(col, max + 1);
					conexiones.push_back(bp);

					if (!conectada)
					{
						conectada = true;
					}
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
	m_connectedWords.clear();

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
	m_connectedWords.clear();
}