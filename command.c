/*
 * =====================================================================================
 *
 *       Filename:  Command.c
 *
 *    Description:  implementation file for command processing 
 *
 *        Version:  1.0
 *        Created:  05/09/2016 08:30:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "command.h"
//define MAX arguments per requirements
#define MAX_ARGV 512

//create command structure to hold arguments, input, output, and foreground/background
struct Command {
    char **argv;
    char *ifile;
    char *ofile;
    int bg;
};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  printCommand
 *  Description:  prints command for debugging purposes 
 * =====================================================================================
 */

void printCommand(Command *c){
	printf("NAME: %s\n", c->argv[0]);
	printf("ARGS: ");
	int i=-1;
	while (c->argv[++i] != 0){
		printf("%s,", c->argv[i]);
	}
	printf("\n");
	printf("IFILE: '%s'\n", c->ifile);
	printf("OFILE: '%s'\n", c->ofile);
	printf("BG: %d\n", c->bg);
	fflush(0);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initCommand
 *  Description:  initializes command with memory and default settings
 * =====================================================================================
 */

void initCommand(Command *c){
    c->argv = (char **) malloc(MAX_ARGV*sizeof(char *));
    c->ifile = 0;
    c->ofile = 0;
    c->bg = 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getArgs
 *  Description:  returns argument array from command structure
 * =====================================================================================
 */

char **getArgs(Command *c){
	return c->argv;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  isBg
 *  Description:  returns true if command is intended as background process
 * =====================================================================================
 */

int isBg(Command *c){
	return c->bg;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getIFile
 *  Description:  returns the input file from command structure
 * =====================================================================================
 */

char *getIFile(Command *c){
	return c->ifile;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getOFile
 *  Description:  returns the output file from command structure
 * =====================================================================================
 */

char *getOFile(Command *c){
	return c->ofile;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parse
 *  Description:  converts user input into command structure
 * =====================================================================================
 */

Command *parse(char *input) {
	//setup command structure
    Command *c = (Command*) malloc(sizeof(Command));
	assert(c != 0);
    initCommand(c);
 
    int end = 0;
    char *token = strtok(input, " ");
	int i = 0;
    do {

	   	//add NULL to end of string array
        if (token == 0){
 			c->argv[i] = 0;	
            end = 1;
        }
		//process input file
        else if (strcmp("<", token) == 0){
            char *ifile = strtok(NULL, " \n");
			if (ifile == 0){
//				c->ifile = (char *) malloc(strlen("/dev/null"));
//				sprintf(c->ifile, "%s", "/dev/null");
			}
			//use /dev/null for empty input on background process
			else if (strcmp("&", ifile) == 0){
				//sets process as background
				c->bg = 1;
				static char null[] = "/dev/null";
				//sets /dev/null on lack of input file
				c->ifile = null;
//				c->ifile = (char *) malloc(strlen("/dev/null"));
//				sprintf(c->ifile, "%s", "/dev/null");
				end = 1;
			}
			else {
				//set input file
            	c->ifile = ifile;
			}
        }
		//process output file
        else if (strcmp(">", token) == 0){
            char *ofile = strtok(NULL, " \n");
			if (ofile == 0){
//				c->ofile = (char *) malloc(strlen("/dev/null"));
//				sprintf(c->ofile, "%s", "/dev/null");
			}
			//sets /dev/null for empty output file on background process
			else if (strcmp("&", ofile) == 0){
				//sets background
				c->bg = 1;
				static char null[] = "/dev/null";
				c->ofile = null;
//				c->ofile = (char *) malloc(strlen("/dev/null"));
//				sprintf(c->ofile, "%s", "/dev/null");
				end = 1;
			}
			else {
				//set proper output file
            	c->ofile = ofile;
			}
        }
		//set command for background process
        else if (strcmp("&", token) == 0){
            c->bg = 1;
            end = 1;
        }
        else {
			//extend string array with next argument
            char *arg = (char *) malloc(strlen(token));
            strcpy(arg, token);
            c->argv[i++] = arg;
        }
        token = strtok(NULL, " ");
 
    }while(!end);

	//added due to hole in first string array NULL termination
	c->argv[i] = NULL; 
    return c;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freeCommand
 *  Description:  decommission command into the ether
 * =====================================================================================
 */

void freeCommand(Command *c){
	int i = -1;
	while (c->argv[++i] != 0){
		free(c->argv[i]);
	}
	free(c->argv);
	free(c);
}


