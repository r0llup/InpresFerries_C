/*
	ServeurTerminaux
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
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "ServeurTerminaux.h"
#include "LibrairieTerminaux.h"

using namespace std;

int hSocketService[MAXCLIENT];
int cptClient = 0;
pthread_mutex_t mutexClient;
pthread_cond_t condClient;

int main(int argc, char *argv[])
{
	int hSocketEcoute;
	int hSocketDupliquee;
	int tailleSockaddr_in = sizeof(struct sockaddr_in);
	socklen_t tailleSocklen_t;
	int posLibre = -1;
	int i = 0;
	int fakeCpt = 0;
	char msgServeur[MAXSTRING];
	struct hostent *infosHost;
	struct in_addr adresseIP;
	struct sockaddr_in adresseSocket;
	pthread_t threadHandle;

	/* Création du fichier de trace */
	createTraceFile(serveurTerminauxLog);

	/* Initialisation des mutex */
	pthread_mutex_init(&mutexClient, NULL);

	/* Initialisation des variables de condition */
	pthread_cond_init(&condClient, NULL);

	/* Lançement des threads */
	for(i=0 ; i < MAXCLIENT ; i++)
	{
		pthread_create(&threadHandle, NULL, fctThread, (void *) i);
	   	pthread_detach(threadHandle);
	}

	/* 1. Création de la socket d'ecoute */
	hSocketEcoute = createSocket();

	if(hSocketEcoute == -1)
	{
		fprintf(stderr, "(MAIN) Ko. createSocket\n");
		return -1;
	}
	fprintf(stderr, "(MAIN) Ok. createSocket\n");

	/* 2. Acquisition des informations sur l'ordinateur local */
	if((infosHost = gethostbyname("localhost")) == 0)
	{
		fprintf(stderr, "(MAIN) Ko. gethostbyname\n");
		close(hSocketEcoute);
		return -1;
	}
	fprintf(stderr, "(MAIN) Ok. gethostbyname\n");

	/* 3. Préparation de la structure sockaddr_in */
	memset(&adresseSocket, NULL, tailleSockaddr_in);
	memset(&adresseSocket, NULL, tailleSocklen_t);
	adresseSocket.sin_family = AF_INET; // Domaine
	adresseSocket.sin_port = htons(PORT_TERMINAL);

	/* 4. Le système prend connaissance de l'adresse et du port de la socket */
	if(bind(hSocketEcoute, (struct sockaddr *) &adresseSocket, tailleSockaddr_in) == -1)
	{
		fprintf(stderr, "(MAIN) Ko. bind\n");
		close(hSocketEcoute);
		return -1;
	}
	fprintf(stderr, "(MAIN) Ok. bind\n");

	for(i=0 ; i < MAXCLIENT ; i++, hSocketService[i] = -1); // init

    do
    {
        /* 5. Mise à l'écoute d'une requête de connexion */
        if(listen(hSocketEcoute, SOMAXCONN) == -1)
	    {
			fprintf(stderr, "(MAIN) Ko. listen\n");
 			close(hSocketEcoute);
		  	return -1;
	   	}
	   	fprintf(stderr, "(MAIN) Ok. listen\n");

	   	/* 6. Acceptation d'une connexion */
	   	if((hSocketDupliquee =
		  	accept(hSocketEcoute, (struct sockaddr *) &adresseSocket, &tailleSocklen_t)) == -1)
	   	{
		  	fprintf(stderr, "(MAIN) Ko. accept\n");
		  	close(hSocketEcoute);
		  	return -1;
	   	}
	   	fprintf(stderr, "(MAIN) Ok. accept\n");

		pthread_mutex_lock(&mutexClient);
	   	if(cptClient >= MAXCLIENT)
	   	{
	       	strcpy(msgServeur, "Err. too much customers");
	       	if(send(hSocketDupliquee, msgServeur, MAXSTRING, NULL) == -1)
	       	{
	           	fprintf(stderr, "(MAIN) Ko. send\n");
	           	close(hSocketDupliquee);
	           	pthread_mutex_unlock(&mutexClient);
	           	return -1;
	       	}
	       	fprintf(stderr, "(MAIN) Ok. send\n");
	       	pthread_mutex_unlock(&mutexClient);
	       	continue;
	   	}
		else
		{
			pthread_mutex_unlock(&mutexClient);
			for(i=0 ; i < MAXCLIENT ; i++)
			{
				if(hSocketService[i] == -1)
					posLibre = i;
			}
			hSocketService[posLibre] = hSocketDupliquee;
			fakeCpt++;
			pthread_mutex_lock(&mutexClient);
			cptClient = fakeCpt;
			pthread_mutex_unlock(&mutexClient);
			pthread_cond_signal(&condClient);
		}
	} while(1);

	/* 9. Fermeture des sockets */
	pthread_mutex_lock(&mutexClient);
	for(i=0 ; i < MAXCLIENT ; i++)
	{
		if(hSocketService[i] != -1)
		{
			close(hSocketService[i]);
			fprintf(stderr, "(MAIN) Ok. close(hSocketService)\n");
		}
	}
	pthread_mutex_unlock(&mutexClient);
	close(hSocketEcoute);
	fprintf(stderr, "(MAIN) Ok. close(hSocketEcoute)\n");

	return 0;
}

int createSocket()
{
	return socket(AF_INET, SOCK_STREAM, NULL);
}

void *fctThread(void *param)
{
	int nbreCarLu = 0, i = 0;
	char msgClient[MAXSTRING];
	char msgServeur[MAXSTRING];
	char *commandeClient, *nomClient, *passwordClient, *numeroTerminalClient;

	while(1)
	{
		pthread_mutex_lock(&mutexClient);
		while(!cptClient)
			pthread_cond_wait(&condClient, &mutexClient);
		pthread_mutex_unlock(&mutexClient);
		strcpy(msgClient, "");
		do
		{
			/* 7. Reception d'un message client */
			if((nbreCarLu = recv(hSocketService[(int) param], msgClient, MAXSTRING, NULL)) == -1)
			{
				fprintf(stderr, "(THREAD_CLIENT) Ko. recv\n");
				close(hSocketService[(int) param]);
				exit(1);
			}
			fprintf(stderr, "(THREAD_CLIENT) Ok. recv\n");
			msgClient[nbreCarLu] = '\0';
			fprintf(stderr, "Message lu : %s\n", msgClient);
			commandeClient = strtok(msgClient, ":");
			if(strcmp(commandeClient, "LOGIN") == 0)
			{
				nomClient = strtok(NULL, ";");
				passwordClient = strtok(NULL, ";");
				numeroTerminalClient = strtok(NULL, ";");
				fprintf(stderr, "commandeClient : %s*\n", commandeClient);
				fprintf(stderr, "nomClient : %s*\n", nomClient);
				fprintf(stderr, "passwordClient : %s*\n", passwordClient);
				fprintf(stderr, "numeroTerminalClient : %s*\n", numeroTerminalClient);
				if (Login("F_AGENTS.csv", nomClient, passwordClient))
					strcpy(msgServeur, "OK");
				else
					strcpy(msgServeur, "KO");
				/* 8. Envoi d'un message serveur */
				if(send(hSocketService[(int) param], msgServeur, MAXSTRING, NULL) == -1)
				{
					fprintf(stderr, "(THREAD_CLIENT) Ko. send\n");
					close(hSocketService[(int) param]);
					exit(1);
				}
				fprintf(stderr, "(THREAD_CLIENT) Ok. send\n");
			}
		} while((strcmp(commandeClient, EOC) != 0) && (strcmp(commandeClient, SGI) != 0));

		hSocketService[(int) param] = -1;
		pthread_mutex_lock(&mutexClient);
		cptClient--;
		pthread_mutex_unlock(&mutexClient);
	}
	pthread_exit(NULL);
}
