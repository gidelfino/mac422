#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>	
#include <pthread.h>
#include <time.h>

#define MAX_SIZE	 1024
#define TIME_TOL	 0.0001

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

int nprocs, nthreads, flags[MAX_SIZE];
clock_t gstart;

struct process { // Struct de processos
	double time, dtime, deadline, timef;
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

int comp_dt(const void *p1, const void *p2) { // Funcao utilizada pelo qsort para ordenacao de processos por tempo a simulado
	const struct process *e1 = p1;
	const struct process *e2 = p2;
	if (e1->dtime > e2->dtime) return 1;
	return 0;
}

void mutexLock() {
	if (pthread_mutex_lock(&mutex) != 0) {
		perror("pthread_mutex_lock()");
		exit(EXIT_FAILURE);
	}
}
void mutexUnlock() {
	if (pthread_mutex_unlock(&mutex) != 0) {
		perror("pthread_mutex_unlock()");
		exit(EXIT_FAILURE);
	}
}

void *realTimeOperation(void *tid) {
	double time, elapsed;
	int id = *((int *) tid);
	clock_t start, end;
	time = procs[id].dtime;

	nthreads++;
	if (nthreads > nprocs - 1)
		mutexLock();
	start = clock();	
	printf("Rodando %d %lf\n", id, time);
	while (1) {
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		if(elapsed >= time) { 
			printf("Thread %d terminou -> Tempo: %lf\n", id, ((double)end - (double)gstart) / CLOCKS_PER_SEC);
			procs[id].timef = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}
	}
	mutexUnlock();	
	nthreads--;
	return NULL;
}

void readTraceFile(char *fn, int *n, Process procs[]) {
	char input[MAX_SIZE];
	FILE *file = fopen(fn, "r");
	if (file != NULL)
		while (fgets(input, sizeof(input), file) != NULL) {
			sscanf(strtok(input, " "), "%lf", &procs[*n].time);
			procs[*n].name = malloc(MAX_SIZE * sizeof(char));
			strcpy(procs[*n].name, strtok(NULL, " "));
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
	FILE *file = fopen(fn, "w");
	if (file != NULL) {
		for (i = 0; i < n; i++) 
			fprintf(file, "%s %lf %lf\n", procs[i].name, procs[i].timef, procs[i].timef - procs[i].time);
		fprintf(file, "%d", cc);
	}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}


int main(int argc, char *argv[]) {
 	int i, rc, n = 0;
	int thread_args[MAX_SIZE];
 	pthread_t threads[MAX_SIZE];
 	clock_t end;

 	nprocs   = sysconf(_SC_NPROCESSORS_ONLN); // numero de processadores do sistema
 	nthreads = 0; 
 	gstart = clock();
 	for (i = 0; i < MAX_SIZE; i++) 
 		flags[i] = 0;

 	if (argc == 4) { // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
  		readTraceFile(argv[2], &n, procs);
		switch (*argv[1]) {
			case '1':
				printf("First-Come First Served.\n");						
				qsort(procs, n, sizeof(Process), comp_t);
				for (i = 0; i < n; ) {	
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
 	for (i = 0; i < n; i++)
 		free(procs[i].name);
 	pthread_mutex_destroy(&mutex);
 	pthread_cond_destroy(&cond);
 	return 0;
}	
