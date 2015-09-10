#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS     5
int nproc, nthreads;
time_t start;

void *TaskCode(void *arg) {
	int id = *((int *) arg);
	int i;
	
	printf("Rodando thread %d\n", id);
	for(i = 0; i < 1000*time; i++);
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int i, rc;
	start = clock();
	//nproc = sysconf(_SC_NPROCESSORS_ONLN);
	nproc = 2;
	nthreads = 1;
	for(i = 0; i < NUM_THREADS; i++) {
		scanf("%d", &thread_args[i]);
		rc = pthread_create(&threads[i], NULL, TaskCode, (void *) &thread_args[i]);
	}
	return 0;
}
