#include <string.h>
#include <stdlib.h>

#include "hash.h"

hash_table_t* init_hash(int table_size) {
    hash_table_t* hash = malloc(sizeof(hash_table_t));
    hash->table = calloc(table_size, sizeof(struct hash_t));
    hash->table_size = table_size;
    return hash;
}

int hash_function(char* key, int hash_size) {
    int result = 0;
    for (int i = 0; i < strlen(key); i++) {
        result = (result * 31 + key[i]) % hash_size;
    }
    return result;
}

void put(hash_table_t* hash_map, char* key, void* value) {
    int index = hash_function(key, hash_map->table_size);
    while ((hash_map->table)[index].key != NULL) {
        index = (index + 1) % hash_map->table_size;
    }
    (hash_map->table)[index].key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy((hash_map->table)[index].key, key);
    (hash_map->table)[index].value = value;
}   

void* get(hash_table_t* hash_map, char* key) {
    int index = hash_function(key, hash_map->table_size);
    while ((hash_map->table)[index].key != NULL && strcmp((hash_map->table)[index].key, key) != 0) {
        index = (index + 1) % hash_map->table_size;
    }
    if ((hash_map->table)[index].key == NULL) {
        return NULL;
    }
    return (hash_map->table)[index].value;
}

void free_hash(hash_table_t* hash_map) {
    for (int i = 0; i < hash_map->table_size; i++) {
        if ((hash_map->table)[i].key != NULL) {
            free((hash_map->table)[i].key);
        }
    }
    free(hash_map->table);
    free(hash_map);
}
