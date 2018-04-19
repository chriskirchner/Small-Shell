/*
 * =====================================================================================
 *
 *       Filename:  Command.h
 *
 *    Description:  header file for command processing
 *
 *        Version:  1.0
 *        Created:  05/09/2016 08:29:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */

#ifndef COMMAND_H
#define COMMAND_H

typedef struct Command Command;
  
void initCommand(Command *c);
Command *parse(char *input);
void freeCommand(Command *c); 
char **getArgs(Command *c);
char *getIFile(Command *c);
char *getOFile(Command *c);
int isBg(Command *c); 
char *ifile(Command *c);
char *ofile(Command *c);
void printCommand(Command *c);

#endif
