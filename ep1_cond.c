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
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int play = 0;
int nthrs, nproc;
clock_t st;

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

void lock() {
	if (pthread_mutex_lock(&mutex) != 0)
		perror("pthread_mutex_lock()");
}
void unlock() {
	if (pthread_mutex_unlock(&mutex) != 0)
		perror("pthread_mutex_unlock()");
}

void *realTimeOperation(void *pid) {
	double time, elapsed;
	int id = *((int *) pid);
	clock_t start, end;
	time = procs[id].dtime;
	//printf("Rodando %d %lf\n", id, time);  
	printf("nthread %d nproc %d\n", nthrs, nproc);
	lock();
	if(nthrs == nproc - 1) {
		printf("Travei no processo %d: %d-%d\n", id, nthrs, nproc);
		play = 0;
	}
	unlock();
	lock();
	while(!play) 
		pthread_cond_wait(&cond, &mutex);
	unlock();
	start = clock();
	printf("Thread %d comecando %lf\n", id, (double)st);
	nthrs++;
	while (1) {
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		if(elapsed >= time) { 
			printf("Thread %d terminou %lf elapsed -> thread %lf\n", id, ((double)end - (double)st) / CLOCKS_PER_SEC, time);
			procs[id].ftime = ((double)end - (double)st) / CLOCKS_PER_SEC;
			break; 
		}
	}
	nthrs--;
	lock();
	//printf("Tentando destravar %d %d\n", nthrs, nproc);
	if(nthrs < nproc - 1) {
		printf("Destravei no processo %d\n", id);
		play = 1;
		pthread_cond_signal(&cond);
	}
	unlock();
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
	int i, rc, n = 0;
	int thread_args[MAX_SIZE];
	pthread_t threads[MAX_SIZE];
	clock_t end;

	nproc = sysconf(_SC_NPROCESSORS_ONLN); // numero de processadores do sistema
	printf("nproc %d\n", nproc);
	if (argc == 4) { // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
		readTraceFile(argv[2], &n, procs);
		st = clock();
		switch (*argv[1]) {
			case '1':
				printf("First-Come First Served.\n");						
				qsort(procs, n, sizeof(Process), comp_t);
				nthrs = 0;
				i = 0; play = 1;
				while(i < n) {	
					end = clock();
					if(((double)end - (double) st) / CLOCKS_PER_SEC  >= procs[i].time - TIME_TOL
							&& ((double)end - (double) st) / CLOCKS_PER_SEC  <= procs[i].time + TIME_TOL) {
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

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}
