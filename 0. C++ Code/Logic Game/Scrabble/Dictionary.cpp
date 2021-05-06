//
//  Dictionary.cpp


#include "Dictionary.h"

#include <fstream>
#include <string>

using namespace std;

Dictionary::Dictionary()
{
	string directorio = "../../1. Resources/data/Dictionaries/";
	string nombreFichero;

	for (int i = 0; i < NO_LANGUAGE; i++)
	{
		switch (i)
		{
		case CATALAN:
			nombreFichero = "Catalan.dic";
			break;
		case SPANISH:
			nombreFichero = "Spanish.dic";
			break;
		case ENGLISH:
			nombreFichero = "English.dic";
			break;
		default:
			break;
		}

		m_languagesPath[i] = directorio + nombreFichero;
	}
}

Dictionary::~Dictionary()
{
	m_words.clear();
}

void Dictionary::setLanguage(Language language)
{
	ifstream fichero;
	fichero.open(m_languagesPath[language]);

	if (fichero.is_open())
	{
		do
		{
			string palabra;
			fichero >> palabra;
			m_words.push_back(palabra);

		} while (!fichero.eof());
	}

	fichero.close();
}

bool Dictionary::check(const string& word)
{
	bool encontrada = false;
	int i = 0;

	while (!encontrada && i < m_words.size())
	{
		if (m_words[i] == word)
		{
			encontrada = true;
		}
		else
		{
			i++;
		}
	}

	return encontrada;
}