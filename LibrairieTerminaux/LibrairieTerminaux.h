/*
	LibrairieTerminaux
	LECLER Quentin
	BLANPAIN Olivier
*/

#ifndef LIBRAIRIETERMINAUX_H
#define LIBRAIRIETERMINAUX_H

const std::string librairieTerminauxLog("LibrairieTerminaux.log");
const std::string whiteSpaces(" \f\n\r\t\v");

void trimRight(std::string& str, const std::string& trimChars);
void trimLeft(std::string& str, const std::string& trimChars);
void trim(std::string& str, const std::string& trimChars);
void createTraceFile(std::string filename);
bool Login(const std::string nomFichier, const std::string nomClient, const std::string passwordClient);

#endif
