#ifndef TYPES_H
#define TYPES_H

typedef enum {
    SILVER,
    GOLD,
    PLATINUM
} process_type_t;

typedef struct {
    char* name;
    int time;
} instruction_t;

struct program_t {
    instruction_t* instruction;
    struct program_t* next;
};

typedef struct {
    char* name;
    int initial_time;
    int arrival_time; // for queueing
    int burst_time; // for statistics
    int priority;
    int finish_time;
    int quantum_time; // for counting quantum
    int quantum_counter; // for upgrading
    process_type_t type;
    struct program_t* program;
} process_t;

#endif