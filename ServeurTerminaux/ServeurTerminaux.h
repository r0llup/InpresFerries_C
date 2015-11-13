/*
	ServeurTerminaux
	LECLER Quentin
	BLANPAIN Olivier
*/

#ifndef SERVEURTERMINAUX_H
#define SERVEURTERMINAUX_H

const std::string serveurTerminauxLog("ServeurTerminaux.log");
#define PORT_TERMINAL 32010
#define MAXSTRING 125
#define EOC "EXIT"
#define SGI "SIGINT"
#define MAXCLIENT 5

int createSocket();
void *fctThread(void *param);

#endif
