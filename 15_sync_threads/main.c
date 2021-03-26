#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define CNT_STORES 5
#define CNT_BUYERS 3

int stores[CNT_STORES];

struct Buyer_info {
	int number; //номер покупателя
	int need; //его потребность в товаре
};

void *func_for_buyer(void *param) {
	//привести параметр функции к правильному типу:
	struct Buyer_info *buyer_info = (struct Buyer_info*) param; 
	printf("I'm buyer %d, my need in goods = %d\n", 
		buyer_info->number, buyer_info->need);
		
	pthread_exit(0);
}

int main(void) {
	srand(time(0));
	//получить дефолтные значения атрибутов:
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	//"создание" магазинов:
	int i;
	for (i = 0; i < CNT_STORES; i++) { 
		stores[i] = 1000 + (rand() % 100 - 50); //1000 +- 50
		printf("stores[%d] have %d goods\n", i, stores[i]);
	}
	
	//"создание" покупателей:
	struct Buyer_info buyers_info[CNT_BUYERS];
	for (i = 0; i < CNT_BUYERS; i++) { //инициализация информации о покупателе
		buyers_info[i].number = i + 1;
		buyers_info[i].need = 5000 + (rand() % 1000 - 500); //5000 +- 500
	}
	pthread_t buyers[CNT_BUYERS]; //массив с id потоков
	for (i = 0; i < CNT_BUYERS; i++) { //создание потоков, которые покупатели
		void *ptr_to_struct = &buyers_info[i];
		if (pthread_create(&buyers[i], &attr, func_for_buyer, ptr_to_struct)) {
			printf("error: can't create new pthread[%d]\n", i);
			exit(1);
		}
	}
	
	//man pthread_mutex_init
	
	//ожидание завершения потоков:
	for (int i = 0; i < CNT_BUYERS; i++) {
		pthread_join(buyers[i], NULL);
	}
	
	exit(EXIT_SUCCESS);
}
