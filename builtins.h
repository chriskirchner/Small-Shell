/*
 * =====================================================================================
 *
 *       Filename:  builtins.h
 *
 *    Description:  header file for built-in functions
 *
 *        Version:  1.0
 *        Created:  05/08/2016 09:27:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "command.h"

int builtin(Command *c, int status, int sig);
void _ls(char *dir);
void _cd(char *dir);
void _status(int status, int sig);

#endif
