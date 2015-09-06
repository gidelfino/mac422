#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
 	int  i = 0;
 	char input[1024];
 	double time[1024];     // instante de tempo em segundos que o processo chega no sistema
 	char   *name[1024];    // string sem espacos que identifica o processo
 	double dtime[1024];    // quanto tempo real da CPU deve ser simulado para o processo
 	double deadline[1024]; // instante de tempo antes do qual o processo precisa terminar
 	int    p[1024];        // prioridade do processo -20 a 19
 	
 	while (fgets(input, sizeof(input), stdin) != NULL) { 	
		sscanf(strtok(input, " "), "%lf", &time[i]);
		name[i] = strtok(NULL, " ");
		sscanf(strtok(NULL, " "), "%lf", &dtime[i]);
		sscanf(strtok(NULL, " "), "%lf", &deadline[i]);
		sscanf(strtok(NULL, " "), "%d", &p[i]);
		printf("%lf %s %lf %lf %d\n", time[i], name[i], dtime[i], deadline[i], p[i]);
		i++;
	}
 	return 0;
}	