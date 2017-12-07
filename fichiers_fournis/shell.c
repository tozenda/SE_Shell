/*
* Copyright (C) 2002, Simon Nieuviarts
*/


/******************************************************************************/
/*			Application: SHELL					*/
/******************************************************************************/
/*									      */
/*			 programme  SHELL				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : FERREIRA Joffrey & OZENDA Thomas 					*/
/*									      */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "readcmd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {

	while (1) {
		struct cmdline *l;
		int spid, spid2;
		int it, it2;

		printf("coquillage> ");
		l = readcmd();

    /* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

    /* Syntax error, read another command */
		if (l->err) {
			printf("error: %s\n", l->err);
			continue;
		}

		for (it = 0; l->seq[it] != NULL; it++) {
		}
		printf("it : %d\n", it);

		int pipes[it - 1][2];
		spid = fork();

		printf("spid : %d\n", spid);
		switch(spid){
		case 0:
			for (it2=0; it2<it; it2++){
				printf("it 2 : %d\n", it2);
        if (l->seq[it2 + 1] != NULL){
					pipe(pipes[it2]);
					spid2 = fork();
				}
				else{
					spid2 = 0;
        }
				printf("spid2 : %d\n", spid2);
				switch (spid2){
				case -1:
					perror("fork");
					exit(-1);
				case 0:
					printf("case 0\n");
           // redirection de la sortie
					if (l->out && it2 == it - 1){
						FILE *fileOut = fopen(l->out, "w");
						dup2(fileno(fileOut), STDOUT_FILENO);
					}

          // redirection de l'entrée
					if (l->in && it2 == 0){
						FILE *fileIn = fopen(l->in, "r");
						dup2(fileno(fileIn), STDIN_FILENO);
					}

          // s'il reste d'autre commandes à éxécuter :
					if (l->seq[it2 + 1] != NULL){
						close(pipes[it2][0]);
						dup2(pipes[it2][1], STDOUT_FILENO);
						close(pipes[it2][1]);
					}
					printf("on arrive à execvp\n");
					execvp(*l->seq[it2], l->seq[it2]);
					exit(0);

				default:
					printf("default it2\n");
					if (l->seq[it2 + 1] != NULL){
						close(pipes[it2][1]);
						dup2(pipes[it2][0], STDIN_FILENO);
						close(pipes[it2][0]);
					}
					break;
				}
			}
			printf("waitpid it2\n");
			waitpid(spid2, NULL, 0);
			break;

    case -1:
			perror("fork error");
			exit(-1);
			break;

		default:
		printf("default / waitpid it\n");
			waitpid(spid, NULL, 0);
			break;
		}
	}
}



