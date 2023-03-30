#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ts_hashmap.h"

pthread_mutex_t *lock;

/**
 * Creates a new thread-safe hashmap. 
 *
 * @param capacity initial capacity of the hashmap.
 * @return a pointer to a new thread-safe hashmap.
 */
ts_hashmap_t *initmap(int capacity) {
  // TODO
  ts_hashmap_t *map = (ts_hashmap_t*) malloc(sizeof(ts_hashmap_t));
  ts_entry_t **table = (ts_entry_t**) malloc(sizeof(ts_entry_t) * capacity);
  map->table = table;
  map->capacity = capacity;
  map->size = 0;
  lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(lock, NULL);
  

  for (int i = 0; i < capacity; i++) {
    table[i] = NULL;
  }

  return map;
}



/**
 * Obtains the value associated with the given key.
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int get(ts_hashmap_t *map, int key) {
  // TODO
  int newIndex = ((unsigned int) key) % map->capacity;
  ts_entry_t *entry = map->table[newIndex];
  
  pthread_mutex_lock(lock);
  while(entry != NULL){
    if(entry->key == key){
      pthread_mutex_unlock(lock);
      return entry->value;
    }
    entry = entry->next;
  }
  
  pthread_mutex_unlock(lock);
  return INT_MAX;
}

/**
 * Associates a value associated with a given key.
 * @param map a pointer to the map
 * @param key a key
 * @param value a value
 * @return old associated value, or INT_MAX if the key was new
 */
int put(ts_hashmap_t *map, int key, int value) {
  // TODO
  int newIndex = ((unsigned int) key) % map->capacity;
  ts_entry_t *entry = map->table[newIndex];

  pthread_mutex_lock(lock);
  while (entry != NULL) {
     if (entry->key == key) {
      // If the key already exists, replace the existing value
      int old = entry->value;
      entry->value = value;
      pthread_mutex_unlock(lock);
      return old;
    }
    entry = entry->next;
  }

  ts_entry_t *new = (ts_entry_t *) malloc(sizeof(ts_entry_t));
  new->key = key;
  new->value = value;
  new->next = map->table[newIndex];
  map->table[newIndex] = new;
  map->size++;

  pthread_mutex_unlock(lock);
  return INT_MAX;
}

/**
 * Removes an entry in the map
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int del(ts_hashmap_t *map, int key) {
  // TODO
  int delIndex = ((unsigned int) key) % map->capacity;
  ts_entry_t *deletee = map->table[delIndex];
  ts_entry_t *previous = NULL;

  pthread_mutex_lock(lock);
  while(deletee != NULL){
    if(deletee->key != key){
      previous = deletee;
      deletee = deletee->next;
    } else{
      break;
    }
  }

  if(deletee != NULL){
    int delVal = deletee->value;
    if(previous != NULL){
      previous->next = deletee->next;
    }else{
      map->table[delIndex] = deletee->next;
    }
    
    map->size--;
    pthread_mutex_unlock(lock);
    return delVal;
  }

  pthread_mutex_unlock(lock);
  return INT_MAX;
}


/**
 * @return the load factor of the given map
 */
double lf(ts_hashmap_t *map) {
  return (double) map->size / map->capacity;
}

/**
 * Prints the contents of the map
 */
void printmap(ts_hashmap_t *map) {
  for (int i = 0; i < map->capacity; i++) {
    printf("[%d] -> ", i);
    ts_entry_t *entry = map->table[i];
    while (entry != NULL) {
      printf("(%d,%d)", entry->key, entry->value);
      if (entry->next != NULL)
        printf(" -> ");
      entry = entry->next;
    }
    printf("\n");
  }
}

void destroy(void){
  pthread_mutex_destroy(lock);
  free(lock);
}