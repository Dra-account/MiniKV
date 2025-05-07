#ifndef STORAGE_H
#define STORAGE_H

typedef struct Storage Storage;

// create and release storage instance
Storage* storage_create();
void storage_free(Storage* storage);

// set key and value
int storage_set(Storage* storage, const char* key, const char* value);

// get the value from key(return an internal static poninter,do not free it)
const char* storage_get(Storage* storage, const char* key);

#endif
