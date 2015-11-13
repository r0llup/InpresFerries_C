/*
	LibrairieTerminaux
	LECLER Quentin
	BLANPAIN Olivier
*/

#include <string>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "LibrairieTerminaux.h"

using namespace std;

void trimRight(std::string& str, const std::string& trimChars = whiteSpaces)
{
   std::string::size_type pos = str.find_last_not_of(trimChars);
   str.erase(pos + 1);
}

void trimLeft(std::string& str, const std::string& trimChars = whiteSpaces)
{
   std::string::size_type pos = str.find_first_not_of(trimChars);
   str.erase(0, pos);
}

void trim(std::string& str, const std::string& trimChars = whiteSpaces)
{
   trimRight(str, trimChars);
   trimLeft(str, trimChars);
}

void createTraceFile(string filename)
{
	int traceFile = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
	dup2(traceFile, fileno(stderr));
}

bool Login(const std::string nomFichier, const std::string nomClient, const std::string passwordClient)
{
	createTraceFile(librairieTerminauxLog);
	ifstream file(nomFichier.c_str(), ios::in);

	if(!file) // tentative d'ouverture du fichier 'nomFichier'
	{
		fprintf(stderr, "Ko. fichier '%s' non trouve", nomFichier.c_str());
		return false;
	}

	else
	{
		char line[25];
		string nomClientFichier;
		string passwordClientFichier;
		int counter = 0;

		while(file.getline(line, 99, ';'))
		{
			if(counter == 4) // fin de ligne
				counter = 0;

			counter++;

			switch(counter)
			{
				case 1: // nomClient
					nomClientFichier = line;
					trim(nomClientFichier, whiteSpaces);
					fprintf(stderr, "nomClient : %s", nomClientFichier.c_str());
					break;
				case 4: // passwordClient
					passwordClientFichier = line;
					trim(passwordClientFichier, whiteSpaces);
					fprintf(stderr, "passwordClientFichier : %s", passwordClientFichier.c_str());
					break;
			}
			if ((nomClient.compare(nomClientFichier) == 0) &&
				(passwordClient.compare(passwordClientFichier) == 0))
				return true;
		}

		file.close();
		fprintf(stderr, "Ok. fichier '%s' charge avec succes", nomFichier.c_str());
	}
	return false;
}
