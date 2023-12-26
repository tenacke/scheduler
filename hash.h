#ifndef HASH_H
#define HASH_H

struct hash_t {
    char* key;
    void* value;
};

typedef struct {
    struct hash_t* table;
    int table_size;
} hash_table_t;

hash_table_t* init_hash(int table_size);
void put(hash_table_t* hash_table, char* key, void* value);
void* get(hash_table_t* hash_table, char* key);
void free_hash(hash_table_t* hash_table);

#endif