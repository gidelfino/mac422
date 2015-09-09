#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>	
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_SIZE	 1024
#define TIME_TOL	 0.0001

sem_t mutex;
clock_t gstart;

struct process { // Struct de processos
	double time, dtime, deadline, ftime;
	char *name;
	int p;
};
typedef struct process Process;
Process procs[MAX_SIZE]; // Array de processos

int comp_t(const void *p1, const void *p2) { // Funcao utilizada pelo qsort para ordenacao de processos por tempo de inicio
	const struct process *e1 = p1;
	const struct process *e2 = p2;
	if (e1->time > e2->time) return 1;
	return 0;
}

void *realTimeOperation(void *pid) {
	double time, elapsed;
	int id = *((int *) pid);
	clock_t start, end;
	time = procs[id].dtime;  

	sem_wait(&mutex);
	start = clock();
	printf("Rodando %d %lf\n", id, time);
	while (1) {
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		if(elapsed >= time) { 
			printf("Thread %d terminou - instante de encerramento: %lf\n", id, (((double)end - (double)gstart) / CLOCKS_PER_SEC));
			procs[id].ftime = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}
	}
	sem_post(&mutex);
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
void writeTraceFile(char *fn, int n, Process procs[], int cc) {
	int i;
	FILE *file = fopen(fn, "r");
	if (file != NULL) {
		for (i = 0; i < n; i++) 
			fprintf(file, "%s %lf %lf\n", procs[i].name, procs[i].ftime, procs[i].ftime - procs[i].time);
		fprintf(file, "%d", cc);
	}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}


int main(int argc, char *argv[]) {
 	int nproc, i, rc, n = 0;
	int thread_args[MAX_SIZE];
 	pthread_t threads[MAX_SIZE];
 	clock_t end;

 	sem_unlink("mutex");
 	sem_init(&mutex, 1, sysconf(_SC_NPROCESSORS_ONLN)); // numero de processadores do sistema
 	gstart = clock();

 	if (argc == 4) { // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
  		readTraceFile(argv[2], &n, procs);
		switch (*argv[1]) {
			case '1':
				printf("First-Come First Served.\n");						
				qsort(procs, n, sizeof(Process), comp_t);
				for (i = 0; i < n;) {	
					end = clock();
					if(((double)end - (double)gstart) / CLOCKS_PER_SEC  >= procs[i].time - TIME_TOL
						&& ((double)end - (double)gstart) / CLOCKS_PER_SEC  <= procs[i].time + TIME_TOL) {
						thread_args[i] = i;
						rc = pthread_create(&threads[i], NULL, realTimeOperation, (void *) &thread_args[i]);
						assert(0 == rc);
						i++;	
					}
				}
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
	for (i = 0; i < n; i++) { //esperando as threads terminarem
		rc = pthread_join(threads[i], NULL);
		assert(0 == rc);
	}
	writeTraceFile(argv[3], n, procs, 4);
 	sem_destroy(&mutex);
 	return 0;
}	
