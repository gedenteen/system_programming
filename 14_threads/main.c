#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define THREADS_CNT 1000
#define ITERATIONS_CNT 10000
long long global_value = 0;

void *my_func(void *param) {
	long long tmp;
	for (int i = 0; i < ITERATIONS_CNT; i++) {
		tmp = global_value;
		tmp++;
		global_value = tmp;
	}
	pthread_exit(0);
}

int main() {
	//получить дефолтные значения атрибутов:
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_t tid[THREADS_CNT];
	//создание потоков:
	for (int i = 0; i < THREADS_CNT; i++) {
		if (pthread_create(&tid[i], &attr, my_func, NULL)) {
			printf("error: can't create new pthread[%d]\n", i);
			exit(1);
		}
	}
	
	//ожидание завершения потоков:
	for (int i = 0; i < THREADS_CNT; i++) {
		pthread_join(tid[i], NULL);
	}
	
	printf("expected global_value == %d\n", THREADS_CNT * ITERATIONS_CNT);
	printf("    real global_value == %lld\n", global_value);
	return 0;
}
