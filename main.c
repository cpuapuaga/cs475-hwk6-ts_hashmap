
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ts_hashmap.h"

ts_hashmap_t *map;

/**
 * 
*/
void* test(void* arg){
	int random;
	int func;
	srand(time(NULL));
	for (int i = 0; i < 1000; i++) {
		random = (int) (rand() % 99 + 1);
		func = (int) (rand() % 3 + 1);
		if(func == 1){
			get(map,random);
		} else if(func == 2){
			put(map, random, random);
		} else{
			del(map,random);
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s <num threads> <hashmap capacity>\n", argv[0]);
		return 1;
	}

  
	int num_threads = atoi(argv[1]);
	int capacity = (unsigned int) atoi(argv[2]);


	map = initmap(capacity);
	pthread_t *threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);

	for (int i = 0; i < num_threads; i++) {
    	pthread_create(&threads[i], NULL, test, NULL);
  	}
	for(int i = 0; i < num_threads; i++){
		pthread_join(threads[i], NULL);
	}

	printmap(map);

	destroy();

	for (int i=0; i<map->capacity; i++) {
		free(map->table[i]);
		map->table[i] = NULL;
	}

	free(map->table);
	map->table = NULL;
	free(map);
	map = NULL;
	free(threads);
	threads = NULL;
	


	return 0;
}

