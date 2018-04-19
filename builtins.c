/*
 * =====================================================================================
 *
 *       Filename:  builtins.c
 *
 *    Description:  implementation file for built-in functions
 *
 *        Version:  1.0
 *        Created:  05/08/2016 08:29:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include "builtins.h"
#include "command.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  builtin
 *  Description:  interface for builtin functions using command struct, last process
 *  			  status, and last signal
 * =====================================================================================
 */

int builtin(Command *c, int status, int sig){
	
	char **args = getArgs(c);
	//process "cd" function
    if (strcmp("cd", args[0]) == 0){
		_cd (args[1]);
    }
	//process "status" function
	else if (strcmp("status", args[0]) == 0){
		_status(status, sig);
	}
	//process "exit" function (does nothing really)
    else if (strcmp("exit", args[0]) == 0){
		return 1;
    }
	else {
		return 0;
	}
    return 1;
} 


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  _cd
 *  Description:  changes directory on directory argument
 * =====================================================================================
 */

void _cd(char *dir){

	//keep track of old pwd using cwd
	static char oldpwd[PATH_MAX];
	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);
	char *home = getenv("HOME");

	//defaults to home directory
	if (dir == 0){
		chdir(home);
	}
	//goes to old pwd on "-" argument
	else if (strcmp("-", dir) == 0){
		chdir(oldpwd);	
	}
	//change to specified absolute/relative directory
	else {
		chdir(dir);
	}
	if (errno == ENOENT){
		printf("cd: No such file or directory\n"); 
	}   

	//track old pwd using cwd on next call
	strcpy(oldpwd, cwd);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  _status
 *  Description:  indicates status of last foreground process
 * =====================================================================================
 */

void _status(int status, int sig){
	if (sig > 0){
		printf("terminated by signal: %d\n", sig);
	}
	else if (status > -1){
		printf("exit value: %d\n", status);
	}
}






