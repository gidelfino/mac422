#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#define MAX_SIZE	 1024

void *realTimeOperation() {
	printf("blah\n");
	return NULL;
}

void readTraceFile(char *fn, int *n, double time[], char *name[], double dtime[], double deadline[], int p[]) {
	char input[MAX_SIZE];
	FILE *file = fopen(fn, "r");
	if (file != NULL)
		while (fgets(input, sizeof(input), file) != NULL) {
			sscanf(strtok(input, " "), "%lf", &time[*n]);
			name[*n] = strtok(NULL, " ");
			sscanf(strtok(NULL, " "), "%lf", &dtime[*n]);
			sscanf(strtok(NULL, " "), "%lf", &deadline[*n]);
			sscanf(strtok(NULL, " "), "%d", &p[*n]);
			*n = *n + 1;
		}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

int main(int argc, char *argv[]) {
 	int  result, i, n = 0;
 	double time[MAX_SIZE];     // instante de tempo em segundos que o processo chega no sistema
 	char   *name[MAX_SIZE];    // string sem espacos que identifica o processo
 	double dtime[MAX_SIZE];    // quanto tempo real da CPU deve ser simulado para o processo
 	double deadline[MAX_SIZE]; // instante de tempo antes do qual o processo precisa terminar
 	int    p[MAX_SIZE];        // prioridade do processo -20 a 19
 	pthread_t threads[MAX_SIZE];

 	if (argc == 4) { // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
  		readTraceFile(argv[2], &n, time, name, dtime, deadline, p);
		switch (*argv[1]) {
			case '1':
				printf("First-Come First Served.\n");
				break;
			case '2':
				printf("Shortest Job First.\n");
				break;
			case '3':
				printf("Shortest Remaining Time Next.\n");
				break;
			case '4':
				printf("Round-Robin.\n");
				break;
			case '5':
				printf("Escalonamento com Prioridade.\n");
				break;
			case '6':
				printf("Escalonamento em Tempo Real com Deadlines Rigidos.\n");
				break;
			default:
				perror("Escalonador Escolhido Incorreto.\n");
				exit(EXIT_FAILURE);
		}
	}
	else {
		printf("Numero incorreto de parametros.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n; i++) { //criando threads
		if (pthread_create(&threads[i], NULL, realTimeOperation, NULL) == 0)
			continue;
		else {
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}
	}
	for (i = 0; i < n; i++) { //esperando as threads terminarem
		if (pthread_join(threads[i], NULL) == 0)
			continue;
		else {
			perror("pthread_join()");
			exit(EXIT_FAILURE);
		}
	}
 	return 0;
}	