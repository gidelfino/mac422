#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {
	pid_t pid;
	int i;
	int status;
    char prompt[1024];
    char input[1024];
    char *token;
    char *command;
    char *parameters[1024];

    while (1) {	// inicio do shell
    	if (getcwd(prompt, sizeof(prompt)) != NULL) // pega e imprime o path no prompt
 			printf("[%s] ", prompt); 
 		else
 			perror("getcwd()");
 		
 		if (fgets(input, sizeof(input), stdin) != NULL) { // pega a entrada do usuario
 			if (input[strlen(input) - 1] == '\n')
 				input[strlen(input) - 1] = '\0';
 		    token = strtok(input, " ");
 		    command = token;
 		    parameters[0] = token;
 		    for (i = 1; token != NULL; i++) {
 		    	token = strtok(NULL, " ");
 		    	parameters[i] = token;
 		    }
 		}
 		else
 			perror("fgets()");

 		if (strcmp(command, "cd") == 0) // caso o comando for "cd"
 			if (parameters[1] != NULL)
 				if (chdir(parameters[1]) != 0)
 					perror("chdir()");
 		if (strcmp(command, "pwd") == 0) // caso o comando for "pwd"
 			printf("%s\n", prompt);

 		switch (pid = fork()) { // executa o processo
			case -1: // erro
				perror("fork()");
				exit(EXIT_FAILURE);
			case 0: // estamos em um processo filho
				status = execve(command, parameters, 0);
				exit(status);
			default: // estamos em um processo pai
				if (waitpid(pid, &status, 0) < 0) {
					perror("waitpid()");
					exit(EXIT_FAILURE);
				}
		}
	}
	return 0;
}
