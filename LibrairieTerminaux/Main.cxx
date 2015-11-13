/*
	LibrairieTerminaux
	LECLER Quentin
	BLANPAIN Olivier
*/

#include <iostream>
#include "LibrairieTerminaux.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Login :" << endl << Login("F_AGENTS.csv", "Sculpturale", "Johnny69") << endl;
	return 0;
}
