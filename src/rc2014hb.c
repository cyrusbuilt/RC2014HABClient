// zcc +cpm -lm -create-app -o rc2014hb.com src/rc2014hb.c lib/sc129.c -DAMALLOC -v

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "lib/sc129.h"

#define VERSION "1.0"

#define SC129_PORT SC129_DEFAULT_ADDRESS

#define STATUS_UNKNOWN       0
#define STATUS_GARAGE_OPEN   128
#define STATUS_GARAGE_CLOSED 64
#define STATUS_GARAGE_AJAR   192
#define STATUS_DISABLED      255

#define CMD_ACTIVATE 1
#define CMD_DISABLE  255
#define CMD_RESET    0

#define SCREEN_HOME           1
#define SCREEN_STATUS         2
#define SCREEN_GARAGE_CONTROL 3

char *status;
int screen = SCREEN_HOME;
volatile bool terminate = false;

void getStatus(int statCode) {
	status = "UNKNOWN STATUS CODE";
	switch (statCode) {
		case STATUS_UNKNOWN:
			status = "STATUS UNKNOWN";
			break;
		case STATUS_GARAGE_OPEN:
			status = "GARAGE OPEN";
			break;
		case STATUS_GARAGE_CLOSED:
			status = "GARAGE CLOSED";
			break;
		case STATUS_GARAGE_AJAR:
			status = "GARAGE AJAR";
			break;
		case STATUS_DISABLED:
			status = "DISABLED";
			break;
		default:
			break;
	}
}

void checkStatus() {
	int input = SC129_read();
	getStatus(input);
}

void showHomeScreen() {
	screen = SCREEN_HOME;
	putchar(12);
	checkStatus();

	fprintf(stdout, "\n\n");
	fprintf(stdout, "************* RC2104 HAB v%s *************\n", VERSION);
	fprintf(stdout, "*                                         *\n");
	fprintf(stdout, "*           By: Cyrus Brunner             *\n");
	fprintf(stdout, "*                                         *\n");
	fprintf(stdout, "* 1) SHOW STATUS                          *\n");
	fprintf(stdout, "* 2) GARAGE CONTROL                       *\n");

	char *stat;
	stat = "DISABLE";
	if (strcmp(status, "DISABLED") == 0) {
		stat = "ENABLE";
	}

	fprintf(stdout, "* 3) %s", stat);

	int spaceCount = 37 - strlen(stat);
	for (int i = 0; i < spaceCount; i++) {
		fprintf(stdout, " ");
	}

	free(stat);

	fprintf(stdout, "*\n");
	fprintf(stdout, "* 4) EXIT                                 *\n");
	fprintf(stdout, "*                                         *\n");
	fprintf(stdout, "*******************************************\n\n");
	fprintf(stdout, "ENTER SELECTION (1/2/3/4): ");
}

void showStatusScreen() {
	screen = SCREEN_STATUS;
	putchar(12);
	checkStatus();

	fprintf(stdout, "\n\n");
	fprintf(stdout, "************* STATUS *************\n");
	fprintf(stdout, "*                                *\n");
	fprintf(stdout, "* STATUS: %s", status);
	int spaces = 23 - strlen(status);
	for (int i = 0; i < spaces; i++) {
		fprintf(stdout, " ");
	}

	fprintf(stdout, "*\n");
	fprintf(stdout, "*                                *\n");
	fprintf(stdout, "* 1) REFRESH                     *\n");
	fprintf(stdout, "* 2) GO BACK                     *\n");
	fprintf(stdout, "*                                *\n");
	fprintf(stdout, "**********************************\n\n");
	fprintf(stdout, "ENTER SELECTION (1/2): ");
}

void showGarageControlScreen() {
	screen = SCREEN_GARAGE_CONTROL;
	putchar(12);
	checkStatus();

	fprintf(stdout, "\n\n");
	fprintf(stdout, "********** GARAGE CONTROL **********\n");
	fprintf(stdout, "*                                  *\n");
	fprintf(stdout, "* 1) ACTIVATE                      *\n");
	fprintf(stdout, "* 2) GO BACK                       *\n");
	fprintf(stdout, "*                                  *\n");
	fprintf(stdout, "* STATUS: %s", status);
	int spaces = 25 - strlen(status);
	for (int i = 0; i < spaces; i++) {
		fprintf(stdout, " ");
	}

	fprintf(stdout, "*\n");
	fprintf(stdout, "*                                  *\n");
	fprintf(stdout, "************************************\n\n");
	fprintf(stdout, "ENTER SELECTION (1/2): ");
}

void toggleEnableState() {
	SC129_write(strcmp(status, "DISABLED") == 0 ? CMD_RESET : CMD_DISABLE);
	msleep(1*18432/4);
}

void activateGarage() {
	SC129_write(CMD_ACTIVATE);
	msleep(1*18432/4);
	SC129_write(CMD_RESET);
}

void getUserInput() {
	char in;
	while (!kbhit()) {;;}
	
	in = toupper(getchar());
	switch(screen) {
		case SCREEN_HOME:
			switch (in) {
				case '1':
					showStatusScreen();
					break;
				case '2':
					showGarageControlScreen();
					break;
				case '3':
					toggleEnableState();
					showHomeScreen();
					break;
				case '4':
					terminate = true;
					break;
				default:
					showHomeScreen();
					break;
			}
			break;
		case SCREEN_STATUS:
			switch (in) {
				case '2':
					showHomeScreen();
					break;
				case '1':
				default:
					showStatusScreen();
					break;
			}
			break;
		case SCREEN_GARAGE_CONTROL:
			switch(in) {
				case '1':
					activateGarage();
					showGarageControlScreen();
					break;
				case '2':
					showHomeScreen();
					break;
				default:
					showGarageControlScreen();
					break;
			}
			break;
	}
}

void loop() {
	while (!terminate) {
		getUserInput();
	}
}

void main(int argc, char **argv) {
	(void)argc;
	(void *)argv;

	SC129_setAddress(SC129_PORT);
	showHomeScreen();

	loop();
	free(status);
	exit(0);
}