#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
   
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = (hashtable_t *) malloc(sizeof(hashtable_t) );
  ht->size = size;
  ht->buckets = (bucket_t **) calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx]; 
  while(b){
    if (strcmp(b->key, key) == 0 ){
      free(b->key);
      free(b->val);
      b->key = key;
      b->val = val;    
      return;
    }
    b = b->next;
  }
  b = (bucket_t *) malloc(sizeof(bucket_t));
  b->val = val;
  b->key = key;
  b->next = ht->buckets[idx];
  ht->buckets[idx] = b;
}

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i< ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  unsigned long i;
  for (i = 0; i < ht->size; i++) {
    bucket_t *b = ht->buckets[i];
    bucket_t *ptr = NULL;
    while (b) {
      ptr = b->next;
      if (b->val) {
        free(b->val);
      }
      if (b->key){
        free(b->key);
      }
      free(b);
      b = ptr;
    }
  }
  free(ht->buckets);
  free(ht); 
}

/* TODO */
void  ht_del(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  bucket_t *ptr = NULL;
  if (strcmp(b->key, key) == 0){
    ht->buckets[idx] = b->next;
    free(b->val);
    free(b->key);
    free(b);
    return;
  }
  while (b && b->next) {
    if(strcmp(b->next->key, key) == 0) {
      ptr = b->next;
      b->next = ptr->next;
      free(ptr->key);
      free(ptr->val);
      free(ptr);
      return;
    }
    b = b->next;
  }
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  hashtable_t *old = (hashtable_t *) malloc(sizeof(hashtable_t));
  bucket_t *b = NULL;
  bucket_t *ptr = NULL;
  unsigned long i;
  old->size = ht->size;
  old->buckets = ht->buckets;
  ht->size = newsize;
  ht->buckets = (bucket_t**) calloc(sizeof(bucket_t *),newsize);
  for (i = 0; i < old->size; i++) {
    b = old->buckets[i];
    while(b) {
      ht_put(ht, b->key, b->val);
      ptr = b;
      b = b->next;
      free(ptr);     
    }
  }
  free(old->buckets);
  free(old);
}
