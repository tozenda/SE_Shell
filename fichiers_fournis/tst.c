/*
* Copyright (C) 2002, Simon Nieuviarts
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "readcmd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


int main()
{
	while (1) {
		struct cmdline *l;
		int i, j;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);
		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			int status,spid;
			switch(spid = fork()){
				case -1 : perror(0);
				exit(-1);
				case 0 :
				if (l->out!=NULL){
						int sortie = open(l->out,O_RDWR);
						if(sortie < 0){
							printf("Fichier invalide\n");
							break;
						}
						dup2(sortie,STDOUT_FILENO);
						close(sortie);
				}
				if (l->in!=NULL){
						int entree = open(l->in,O_RDWR);
						if(entree < 0){
							printf("Fichier invalide\n");
							break;
						}
						dup2(entree,STDIN_FILENO);
						close(entree);
				}
				execvp(cmd[0],cmd);
				break;
				default : // le pere attend la terminaison du fils
				if (waitpid(spid,&status,0)==-1) {
					perror(0);
					exit(-1);
				}
				break;
				printf("\n");
			}
		}
	}
}
