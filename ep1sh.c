#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char **argv, char **envp) {
	pid_t pid;
	int i;
	int status;
    char path[1024];
    char prompt[1024];
	char *input;
    char *token;
    char *command;
    char *parameters[1024];

    while (42) {	// inicio do shell
    	if (getcwd(path, sizeof(path)) != NULL) {
    		strcpy(prompt, "");
			strcat(strcat(strcat(prompt, "["), path), "] ");
			input = readline(prompt); // impressao do prompt leitura do input do usuario 
			if (input[strlen(input) - 1] == '\n')
 				input[strlen(input) - 1] = '\0';
			if(input != NULL) {		  // adicao do input ao historico
				add_history(input);
				token = strtok(input, " ");
				command = token;	  // separacao do input entre comando e parametros
				parameters[0] = token;
				for (i = 1; token != NULL; i++) {
					token = strtok(NULL, " ");
					parameters[i] = token;
				}
			}
			else 
				perror("readline()");
		}
 		else
 			perror("getcwd()");

 		if (strcmp(command, "cd") == 0) {     // cd <novo diretorio para mudar>
 			if (parameters[1] != NULL)
 				if (chdir(parameters[1]) != 0)
 					perror("chdir()");
 		}
 		else if (strcmp(command, "pwd") == 0) // pwd
 			printf("%s\n", prompt);
 		else                                  // /bin/ls -1     e    ./ep1 <argumentos do EP1>
 			switch (pid = fork()) {
				case -1: // erro
					perror("fork()");
					exit(EXIT_FAILURE);
				case 0:  // estamos em um processo filho
					status = execve(command, parameters, 0);
					exit(status);
				default: // estamos em um processo pai 
					if ((waitpid(pid, &status, 0)) < 0) {
						perror("waitpid()");
						exit(EXIT_FAILURE);
					}
			}
	}
	return 0;
}
