/*
 * =====================================================================================
 *
 *       Filename:  smallish.c
 *
 *    Description:  main implementation file for a small (tiny) shell 
 *
 *        Version:  1.0
 *        Created:  05/08/2016 12:15:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

//custom headers for builtin functions and command processing
#include "builtins.h"
#include "command.h"

//keeps track of last exit status and signal
//really need to use globals for old school sig handlers?
int status;
int signum;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sigint
 *  Description:  signal handler for SIGINT signals from foreground processes
 * =====================================================================================
 */

void sigint(int sig){
	int stat;
	pid_t pid;
	//wait zombie to death from SIGINT
	if ((pid = wait(&stat)) > 0){
		printf("terminated by signal %d\n", sig);
		fflush(0);
		//tracks last signal sent
		signum = sig;
	}
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sigchld
 *  Description:  signal handler for SIGCHLD signals from background processes
 * =====================================================================================
 */
	
void sigchld(int sig){
	pid_t pid;
	int stat;
	//wait zombie to death without waiting too long
	if ((pid = waitpid(-1, &stat, WNOHANG)) > 0){
		//check if termination signal was sent
		if (WIFSIGNALED(stat)){
			//record termination signal
			signum = WTERMSIG(stat);
			printf("background pid %d is done: terminated by signal %d\n",
					pid, signum);
			fflush(0);
		}
		//check if process exited
		else if (WIFEXITED(stat)){
			printf("background pid %d is done: exit value %d\n",
					pid, stat);
			fflush(0);
		}
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  the one and only
 * =====================================================================================
 */

	int
main ( int argc, char *argv[] )
{

	//setup SIGINT signal action for foreground processes
	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = SIG_IGN;
	sigfillset(&act.sa_mask);
	sigaction(SIGINT, &act, 0);

	//setup SIGCHLD signal action for background processes
	struct sigaction chld;
	chld.sa_flags = 0;
	chld.sa_handler = sigchld;
	sigfillset(&chld.sa_mask);
	sigaction(SIGCHLD, &chld, 0);

	//setup input mask so only SIGINT can be received
	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGINT);
	
	//setup variables; input size based on requirements
	char input[2049]; 
	char **args = 0;	
	Command *command = 0;
	pid_t pid;

	do {

		//block signals from messing up input aside from SIGINT
		sigprocmask(SIG_BLOCK, &mask, 0);

		//prompt
		printf(": ");
		fflush(0);
	
		//read input	
		fgets(input, sizeof(input)-1, stdin);
		strtok(input, "\n");

		//free command structure of dynamic memory
		if (command != 0){
			freeCommand(command);
		}
		command = parse(input);

		//get arguments from command input
		args = getArgs(command);

		//process commands that need forking and exec
		if (!builtin(command, status, signum) && 
				strcmp("\n", input) != 0 &&
				strcmp("#", &input[0])){
			
			//create child
			if ((pid = fork()) == 0){
				//unblock and process any pending signals
				sigprocmask(SIG_UNBLOCK, &mask, 0);

				//setup background process
				if (isBg(command)){
					//ignore SIGINT for background processes
					act.sa_handler = SIG_IGN;
					sigaction(SIGINT, &act, 0);
					//redirect standard output to /dev/null instead of screen
					static char null[] = "/dev/null";
					int nullfd = open(null, O_WRONLY);
					if (dup2(nullfd, 1) == -1){
						perror("no redirection\n");
					}
				}
				//setup foreground process
				else {
					//terminates on SIGINT
					act.sa_handler = SIG_DFL;
					sigaction(SIGINT, &act, 0);
				}

				//setup file redirection
				char *file = 0;

				//process any output file present in command
				int ofd;
				if ((file = getOFile(command)) != 0){
					//open fd for writing
					ofd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0644);	
					if (ofd == -1){
						printf("faled to open %s file for writing", file);
						fflush(0);
						exit(1);
					}
					//redirect output file to standard output
					dup2(ofd, 1);
				}

				//process any input file present in command
				int ifd;
				if ((file = getIFile(command)) != 0){
					//open fd for reading
					ifd = open(file, O_RDONLY);
					if (ifd == -1){
						printf("cannot open %s for input\n", file);
						fflush(0);
						exit(1);
					}
					//redirect input file to standard input
					dup2(ifd, 0);
				}

				//convert fork to executing program
				int eval = execvp(args[0], args);

				//error processing if no program exists
				if (errno == ENOENT){
					printf("%s: no such file or directory\n", args[0]);
					fflush(0);
					exit(1);
				}
				//catch all other errors
				else if (eval == -1){
					perror("smallish: cannot execute file");
					fflush(0);
					exit(1);
				}
				exit(0);

			}

			//process command in foreground
			else if (pid > 0 && !isBg(command)) {
				//allow foreground to react to SIGINT
				act.sa_handler = sigint;
				sigaction(SIGINT, &act, 0);
				//wait foreground process to death
				if (waitpid(pid, &status, 0) > 0){
					//unblock signals and processing any pending
					sigprocmask(SIG_UNBLOCK, &mask, 0);
					//get exit status of foreground process
					status = WEXITSTATUS(status);
					signum = 0;
				}

				//stop reacting to SIGINT
				act.sa_handler = SIG_IGN;
				sigaction(SIGINT, &act, 0);
			}
			//indicated background process id
			else if (pid > 0 && isBg(command)){
				printf("background pid is %d\n", pid);
				fflush(0);
				//unblock signals and processing any pending
				sigprocmask(SIG_UNBLOCK, &mask, 0);
			}
			else {
				perror("Umm?");
				exit(EXIT_FAILURE);
			}
		}	
		//unblock signals and processing any pending
		sigprocmask(SIG_UNBLOCK, &mask, 0);


	}while(!strcmp("exit", args[0]) == 0);

	//kill all remaining children (doesn't that sound bad?)
	int stat;
	act.sa_handler = SIG_IGN;
	sigaction(SIGTERM, &act, 0);
	kill(0, SIGTERM);
	while(waitpid(-1, &stat, 0) > 0){};

	//kill self (sounds just as bad)
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

