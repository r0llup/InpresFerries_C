/*
	ApplicTerminaux
	LECLER Quentin
	BLANPAIN Olivier
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "ApplicTerminaux.h"
#include "LibrairieTerminaux.h"

using namespace std;

int hSocket;

int main(int argc, char *argv[])
{
	int tailleSocketaddr_in = sizeof(struct sockaddr_in);
	struct hostent *infosHost;
	struct in_addr adresseIP;
	struct sockaddr_in adresseSocket;
	struct sigaction act;
	char msgClient[MAXSTRING];
	char msgServeur[MAXSTRING];
	int login = 0;
	char commandeClient[25];
	char nomClient[25];
	char passwordClient[25];
	char numeroTerminalClient[25];

	/* Création du fichier de trace */
	createTraceFile(applicTerminauxLog);

	/* 1. Création de la socket */
	hSocket = createSocket();

	if(hSocket == -1)
	{
		fprintf(stderr, "Ko. createSocket\n");
		return -1;
	}
	fprintf(stderr, "Ok. createSocket\n");

	/* 2. Acquisition des informations sur l'ordinateur local */
	if((infosHost = gethostbyname("localhost")) == 0)
	{
		fprintf(stderr, "Ko. gethostbyname\n");
		close(hSocket);
		return -1;
	}
	fprintf(stderr, "Ok. gethostbyname\n");

	/* 3. Préparation de la structure sockaddr_in */
	memset(&adresseSocket, NULL, tailleSocketaddr_in);
	adresseSocket.sin_family = AF_INET; // Domaine
	adresseSocket.sin_port = htons(PORT_TERMINAL);

	/* 4. Tentative de connexion */
	if((connect(hSocket, (struct sockaddr *) &adresseSocket, tailleSocketaddr_in)) == -1)
	{
		fprintf(stderr, "Ko. connect\n");
		close(hSocket);
		return -1;
	}
	fprintf(stderr, "Ok. connect\n");

	/* Armement du signal SIGINT */
	act.sa_handler = handlerSigInt;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	do
	{
		// LOGIN
		if(!login)
		{
			strcpy(msgClient, "LOGIN:");
			cout << "Veuillez vous identifier :" << endl;
			cout << "Nom : ";
			gets(nomClient);
			strcat(msgClient, nomClient);
			strcat(msgClient, ";");
			cout << "Password : ";
			gets(passwordClient);
			strcat(msgClient, passwordClient);
			strcat(msgClient, ";");
			cout << "Numero de terminal : ";
			gets(numeroTerminalClient);
			strcat(msgClient, numeroTerminalClient);
			strcat(msgClient, ";");

			/* 5. Envoi d'un message client */
			if(send(hSocket, msgClient, MAXSTRING, NULL) == -1)
			{
				fprintf(stderr, "Ko. send\n");
				close(hSocket);
				return -1;
			}
			fprintf(stderr, "Ok. send\n");

			/* 6. Reception de l'ACK du serveur au client */
			if(recv(hSocket, msgServeur, MAXSTRING, NULL) == -1)
			{
				fprintf(stderr, "Ko. recv\n");
				close(hSocket);
				return -1;
			}
			fprintf(stderr, "Ok. recv\n");

			if(strcmp(msgServeur, "OK") == 0)
			{
				fprintf(stderr, "Ok. login\n");
				cout << "Bienvenue " << nomClient << " :)" << endl;
				login = 1;
			}

			else
			{
				fprintf(stderr, "Ko. login\n");
				cout << "Informations de login incorrecte :(" << endl;
			}
		}

		else
		{
			cout << "Veuillez encoder une commande :" << endl;
			gets(msgClient);

			if (strcmp(msgClient, EOC) == 0)
			{
				/* 5. Envoi d'un message client */
				if(send(hSocket, msgClient, MAXSTRING, NULL) == -1)
				{
					fprintf(stderr, "Ko. send\n");
					close(hSocket);
					return -1;
				}

				fprintf(stderr, "Ok. send\n");
			}
		}
	} while(strcmp(msgClient, EOC) != 0);

	/* 7. Fermeture de la socket */
	close(hSocket);

	return 0;
}

int createSocket()
{
	return socket(AF_INET, SOCK_STREAM, NULL);
}

void handlerSigInt(int sig)
{
	char msgClient[MAXSTRING];
	sprintf(msgClient, "%s:", SGI);

	/* Envoi d'un message client */
	if(send(hSocket, msgClient, MAXSTRING, NULL) == -1)
	{
		fprintf(stderr, "Ko. send\n");
		close(hSocket);
		exit(1);
	}

	fprintf(stderr, "Ok. send\n");
	/* Fermeture de la socket */
	close(hSocket);
	exit(0);
}
