#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>	
#include <pthread.h>
#include <time.h>

#define MAX_SIZE	 1024
#define TIME_TOL	 0.000001

struct process {
	double time, dtime, deadline, ftime;
	char *name;
	int p;
};
typedef struct process Process;

int comp_1(const void *p1, const void *p2) {
	const struct process *e1 = p1;
	const struct process *e2 = p2;
	if(e1->time > e2->time) return 1;
	return 0;
}

void *realTimeOperation() {
	printf("blah\n");
	return NULL;
}

void readTraceFile(char *fn, int *n, Process procs[]) {
	char input[MAX_SIZE];
	FILE *file = fopen(fn, "r");
	if (file != NULL)
		while (fgets(input, sizeof(input), file) != NULL) {
			sscanf(strtok(input, " "), "%lf", &procs[*n].time);
			procs[*n].name = strtok(NULL, " ");
			sscanf(strtok(NULL, " "), "%lf", &procs[*n].dtime);
			sscanf(strtok(NULL, " "), "%lf", &procs[*n].deadline);
			sscanf(strtok(NULL, " "), "%d", &procs[*n].p);
			*n = *n + 1;
		}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

int main(int argc, char *argv[]) {
 	int  result, nproc, i, n = 0;
	Process procs[MAX_SIZE];
 	pthread_t threads[MAX_SIZE];
 	clock_t start, end, elapsed;

 	start = clock();
 	nproc = sysconf(_SC_NPROCESSORS_ONLN); // numero de CPU's do sistema
 	
 	if (argc == 4) { // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
  		readTraceFile(argv[2], &n, procs);
		qsort(procs, n, sizeof(Process), comp_1);
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
				printf("Escalonador Escolhido Incorreto.\n");
				exit(EXIT_FAILURE);
		}
	}
	else {
		printf("Numero incorreto de parametros.\n");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < n; i++) //esperando as threads terminarem
		result = pthread_join(threads[i], NULL);
		assert(0 == result);
 	return 0;
}	
