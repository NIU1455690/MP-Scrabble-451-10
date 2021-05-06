//
//  Dictionary.cpp


#include "Dictionary.h"

#include <fstream>
#include <string>

using namespace std;

Dictionary::Dictionary()
{
}

Dictionary::~Dictionary()
{
	m_words.clear();
}

void Dictionary::setLanguage(Language language)
{
	ifstream fitxer;
	string nomFitxer = "";
	string paraula = "";

	switch (language)
	{
	case CATALAN:
		nomFitxer = "Catalan.dic";
		break;
	case SPANISH:
		nomFitxer = "Spanish.dic";
		break;
	case ENGLISH:
		nomFitxer = "English.dic";
		break;
	default:
		break;
	}

	fitxer.open(nomFitxer);

	m_words.resize(1);
	fitxer >> paraula;
	m_words[0] = paraula;

	while (fitxer.is_open())
	{
		fitxer >> paraula;
		m_words.push_back(paraula);
	}

	fitxer.close();
}

bool Dictionary::check(const string& word)
{
	bool encontrada = false;

	for (int i = 0; i < m_words.size() && !encontrada; i++)
	{
		if (m_words[i] == word)
		{
			encontrada = true;
		}
	}

	return encontrada;
}