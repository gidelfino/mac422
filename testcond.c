#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS     6
int nproc, nthread, play, next;
int flag[NUM_THREADS];
time_t start;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void threadResumeAll() {
	pthread_mutex_lock(&mutex);
	play = 1;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

void threadResumeNext() {
	pthread_mutex_lock(&mutex);
	flag[next] = 1;
	if(next > 0) next--;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

void threadSuspendAll() {
	pthread_mutex_lock(&mutex);
	play = 0;
	pthread_mutex_unlock(&mutex);
}

void threadStatus(int id) {
	pthread_mutex_lock(&mutex);
	while(!play) pthread_cond_wait(&cond, &mutex);
	while(!flag[id]) pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);
}

void lock() {
	pthread_mutex_lock(&mutex);
}

void unlock() {
	pthread_mutex_unlock(&mutex);
}

void *TaskCode(void *arg) {
	time_t st, end;
	double elaps;
	int id = *((int *) arg);	
	if(nthread == nproc) threadSuspendAll();
	threadStatus(id);
	lock(); nthread++; unlock();
	printf("Rodando thread %d\n", id);
	st = clock();
	while(42) {
		end = clock();
		elaps = ((double)end - (double)st) / CLOCKS_PER_SEC;
		if(elaps >= 10.0) break;
	}
	printf("Terminei thread %d com %lf\n", id, ((double)end - (double)start) / CLOCKS_PER_SEC);
	nthread--;
	threadResumeNext();
	threadResumeAll();
	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int i, rc;
	//nproc = sysconf(_SC_NPROCESSORS_ONLN);
	nproc = 4;
	nthread = 0;
	play = 1;
	for(i = 0; i < NUM_THREADS; i++) { 
		thread_args[i] = i + 1;
		flag[i + 1] = 0;
	}
	flag[6] = flag[5] = 1; 
	next = 4;
	start = clock();
	for(i = 0; i < NUM_THREADS; i++) {
		rc = pthread_create(&threads[i], NULL, TaskCode, (void *) &thread_args[i]);
		assert(0 == rc);
	}
	for (i=0; i<NUM_THREADS; ++i) {
		rc = pthread_join(threads[i], NULL);
		assert(0==rc);
	}
	return 0;
}
