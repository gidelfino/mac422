#include <stdio.h>
#include <stdlib.h>

struct alo {
	int a, b;
};

typedef struct alo Alo;

int comp(const void *a, const void *b) {
	const struct alo *elem1 = a;    
	const struct alo *elem2 = b;
	if(elem1->a < elem2->a) return 1;
	return 0;
}

int main() {
	int i;
	Alo v[10];
	for(i = 0; i < 5; i++) 
		scanf("%d %d", &v[i].a, &v[i].b);
	qsort(v, 5, sizeof(Alo), comp);
	for(i = 0; i < 5; i++)
		printf("%d %d\n", v[i].a, v[i].b);
	return 0;
}
