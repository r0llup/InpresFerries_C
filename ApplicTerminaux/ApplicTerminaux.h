/*
	ApplicTerminaux
	LECLER Quentin
	BLANPAIN Olivier
*/

#ifndef APPLICTERMINAUX_H
#define APPLICTERMINAUX_H

const std::string applicTerminauxLog("ApplicTerminaux.log");
#define PORT_TERMINAL 32010
#define MAXSTRING 125
#define EOC "EXIT"
#define SGI "SIGINT"

int createSocket();
void handlerSigInt(int sig);

#endif
