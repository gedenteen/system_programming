#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CNT_STORES 5
#define CNT_BUYERS 3

int stores[CNT_STORES];
pthread_mutex_t mutexes[CNT_STORES];
int satisfied_needs = 0;

struct Buyer_info {
	int number; //номер покупателя
	int need; //его потребность в товаре
};

void *func_for_buyer(void *param) {
	//привести параметр функции к стурктуре Buyer_info:
	struct Buyer_info *buyer_info = (struct Buyer_info*) param; 
	printf("I'm buyer %d, my need in goods = %d\n", 
		buyer_info->number, buyer_info->need);

	//цикл, пока не удовлетвоверена потребность в товаре:
	while (buyer_info->need > 0) {
		sleep(2);
		int ind = rand() % CNT_STORES; //выбирается случайный мазагин
		pthread_mutex_lock(&mutexes[ind]);
			printf("I'm buyer %d, visite the store %d, buy %4d goods, my need now = %4d\n",
				buyer_info->number, ind + 1, stores[ind], buyer_info->need - stores[ind]);
			buyer_info->need -= stores[ind];
			stores[ind] = 0;
		pthread_mutex_unlock(&mutexes[ind]);
		
	}
		
	printf("I'm buyer %d and I satisfied my needs\n", buyer_info->number);
	satisfied_needs++;		
	pthread_exit(0);
}

void *func_for_loader(void *param) {
	while (satisfied_needs < CNT_BUYERS) {
		int ind = rand() % CNT_STORES; //выбирается случайный мазагин
		pthread_mutex_lock(&mutexes[ind]);
			stores[ind] += 500;
			printf("I'm loader, add goods for the store %d, now count of goods there = %d\n",
				ind + 1, stores[ind]); 
		pthread_mutex_unlock(&mutexes[ind]);
		sleep(1);
	}
	pthread_exit(0);
}

int main(void) {
	srand(time(0));
	//получить дефолтные значения атрибутов:
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	//создание магазинов (инициализация массива):
	int i;
	for (i = 0; i < CNT_STORES; i++) { 
		stores[i] = 1000 + (rand() % 100 - 50); //1000 +- 50
		printf("store %d have %d goods\n", i + 1, stores[i]);
	}
	//создание мьютексов для магазинов:
	for (i = 0; i < CNT_STORES; i++) {
		if (pthread_mutex_init(&mutexes[i], NULL)) {
			printf("error: can't create mutexes[%d] (mutex)\n", i);
			exit(1);
		}
	}
	//создание информации о покупателях:
	struct Buyer_info buyers_info[CNT_BUYERS];
	for (i = 0; i < CNT_BUYERS; i++) { //инициализация информации о покупателе
		buyers_info[i].number = i + 1;
		buyers_info[i].need = 5000 + (rand() % 1000 - 500); //5000 +- 500
	}
	//создание покупателей (создание потоков):
	pthread_t buyers[CNT_BUYERS]; //массив с id потоков
	for (i = 0; i < CNT_BUYERS; i++) { //создание потоков, которые покупатели
		void *ptr_to_struct = &buyers_info[i];
		if (pthread_create(&buyers[i], &attr, func_for_buyer, ptr_to_struct)) {
			printf("error: can't create buyers[%d] (pthread)\n", i);
			exit(1);
		}
	}
	//создание погрузщика:
	pthread_t loader;
	if (pthread_create(&loader, &attr, func_for_loader, NULL)) {
		printf("error: can't create loader (pthread)\n");
		exit(1);
	}
	//ожидание завершения потоков:
	for (int i = 0; i < CNT_BUYERS; i++) {
		pthread_join(buyers[i], NULL);
	}
	pthread_join(loader, NULL);
	//уничтожение мьютексов:
	for (int i = 0; i < CNT_STORES; i++) {
	    pthread_mutex_destroy(&mutexes[i]);
	}
	
	exit(EXIT_SUCCESS);
}
