#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>	
#include <pthread.h>
#include <time.h>

#define MAX_SIZE	 1024
#define TIME_TOL	 0.1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

int nprocs, nthreads, dflag, flags[MAX_SIZE];
clock_t gstart;

struct process { // Struct de processos
	double time, dtime, deadline, timef;
	char *name;
	int p;
	int traceline;
};
typedef struct process Process;
Process procs[MAX_SIZE]; // Array de processos

int comp_t(const void *p1, const void *p2) { // Funcao utilizada pelo qsort para ordenacao de processos por tempo de inicio
	const struct process *e1 = p1;
	const struct process *e2 = p2;
	if (e1->time > e2->time) return 1;
	return 0;
}

int comp_dt(const void *p1, const void *p2) { // Funcao utilizada pelo qsort para ordenacao de processos por tempo a ser simulado
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

void threadSuspend(int tid) { // Funcao que suspende o processamento de uma thread
	mutexLock();
	flags[tid] = 0;
	mutexUnlock();
}
void threadResume(int tid) { // Funcao que resume o processamento de uma thread
	mutexLock();
	flags[tid] = 1;
	if (pthread_cond_signal(&cond) != 0) {
		perror("pthread_cond_signal()");
		exit(EXIT_FAILURE);
	}
	mutexUnlock();
}
void threadStatus(int tid) { // Funcao que verifica o estado de uma thread (suspensa ou nao)
	while (flags[tid] == 0)
		if (pthread_cond_wait(&cond, &mutex) != 0) {
			perror("pthread_cond_wait()");
			exit(EXIT_FAILURE);
		}
}

void readTraceFile(int argc, char *fn, int *n, Process procs[]) { // Funcao de leitura do arquivo de trace
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
			procs[*n].traceline = *n + 1;
			*n = *n + 1;
		}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}
void writeTraceFile(char *fn, int n, Process procs[], int cc) { // Funcao de escrita do arquivo de trace
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

void *realTimeOperation(void *tid) { // Funcao que realiza uma operacao que consuma tempo real da CPU
	double time, elapsed;
	int id = *((int *) tid);
	clock_t start, end;
	time = procs[id].dtime;
	
	if (nthreads >= nprocs )
		mutexLock();
	mutexLock(); nthreads++; mutexUnlock();
	start = clock();
	printf("Rodando %d %lf\n", id, time);
	while (1) {
		threadStatus(id);
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		if (elapsed >= time) { 
			printf("Thread %d terminou -> Tempo: %lf\n", id, ((double)end - (double)gstart) / CLOCKS_PER_SEC);
			procs[id].timef = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}
	}		
	mutexUnlock();
	nthreads--;
	if (dflag == 1)
		printf("Processo da linha [%d] finalizado, escrito na linha [%d]\n", procs[id].traceline, id + 1);
	return NULL;
}

void firstCome(int n, pthread_t threads[]) {
	int i, rc, thread_args[MAX_SIZE];
	clock_t end, elapsed;
	
	printf("First-Come First Served.\n");						
	for (i = 0; i < n; ) {	
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		if (elapsed >= procs[i].time - TIME_TOL && elapsed  <= procs[i].time + TIME_TOL) {
			if (dflag == 1)
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].traceline);
			thread_args[i] = i;
			rc = pthread_create(&threads[i], NULL, realTimeOperation, (void *) &thread_args[i]); 
			assert(0 == rc);
			i++;
		}
	}
}
/*s
void shortestJob(int n, pthread_t threads[]) {
	int i, rc, thread_args[MAX_SIZE];
	clock_t end, elapsed;

	printf("Shortest Job First.\n");
	i = 0;
	while (nthreads < nprocs && i < n) {	
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		if (elapsed >= procs[i].time - TIME_TOL && elapsed  <= procs[i].time + TIME_TOL) {
			if (dflag == 1)
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].traceline);
			thread_args[i] = i;
			rc = pthread_create(&threads[i], NULL, realTimeOperation, (void *) &thread_args[i]); 
			assert(0 == rc);
			i++;
		}
	}
	while (i < n) {
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		if (elapsed >= procs[i].time - TIME_TOL && elapsed  <= procs[i].time + TIME_TOL) {
			if (dflag == 1)
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].traceline);
			// coloque o processo na fila
		if (nthreads < nprocs) {
			// criar uma thread para o primeiro processo da fila
			i++
		}
		else
			// qsort comp_dt na fila

	}
}
*/

int main(int argc, char *argv[]) {
 	int i, rc, n = 0;
 	pthread_t threads[MAX_SIZE];

 	nprocs   = sysconf(_SC_NPROCESSORS_ONLN); // numero de processadores do sistema 
 	gstart   = clock();
 	nthreads = 0;
 	dflag 	 = 0;
 	
 	for (i = 0; i < MAX_SIZE; i++) 
 		flags[i] = 1;

 	if (argc >= 4) {   // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
  		if (argc == 5) //			  4- (opcional) caracter d
  			if (strcmp(argv[4], "d") == 0)
  				dflag = 1;
  		
  		readTraceFile(argc, argv[2], &n, procs);
  		qsort(procs, n, sizeof(Process), comp_t);

		switch (*argv[1]) {
			case '1':
				firstCome(n, threads);
				break;
			case '2':
				//shortestJob(n, threads);
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
	if (dflag == 1) 
		printf("Numero de mudancas de contexto: %d\n", 0);
	writeTraceFile(argv[3], n, procs, 0);
 	for (i = 0; i < n; i++)
 		free(procs[i].name);
 	pthread_mutex_destroy(&mutex);
 	pthread_cond_destroy(&cond);
 	return 0;
}	
