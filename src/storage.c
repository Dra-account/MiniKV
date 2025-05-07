#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/storage.h"

#define MAX_ITEMS 1024
#define KEY_SIZE 128
#define VALUE_SIZE 256


typedef struct {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
} KvPair;

struct Storage {
    KvPair items[MAX_ITEMS];
    int count;
};

Storage* storage_create() {
    Storage* s = malloc(sizeof(Storage));
    if(s) {
        s->count = 0;
    }
    return s;
}

void storage_free(Storage* s){
    free(s);
}

int storage_set(Storage* s, const char* key, const char* value) {
    for (int i = 0; i < s -> count; ++i){
        if (strcmp(s -> items[i].key, key) == 0) {
	    strncpy(s -> items[i].value, value, VALUE_SIZE);
	    return 0;
	}
    }
    if (s -> count >= MAX_ITEMS) return -1;
    strncpy(s -> items[s -> count].key, key, KEY_SIZE);
    strncpy(s -> items[s -> count].value, value, VALUE_SIZE);
    s -> count++;
    return 0;
}

const char* storage_get(Storage* s, const char* key) {
    for (int i = 0; i < s -> count; ++i) {
        if (strcmp(s -> items[i].key, key) == 0){
	    return s -> items[i].value;
	}
    }
    return NULL;
}
