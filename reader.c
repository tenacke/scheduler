#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "hash.h"
#include "heap.h"
#include "types.h"

int ready_queue_compare(void* a, void* b);
int wait_queue_compare(void* a, void* b);

hash_table_t* instructions;
heap_t* ready_queue;
heap_t* wait_queue;

int init_scheduler(char* instructions_file, char* program_file){
    // initialize instructions and processes hash tables
    ready_queue = init_heap(10, sizeof(process_t), &ready_queue_compare);
    wait_queue = init_heap(10, sizeof(process_t), &wait_queue_compare);
    instructions = init_hash(50);
    read_instructions(instructions_file);
    return read_definitions(program_file);
}

void read_instructions(char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File %s not found\n", filename);
        exit(1);
    }
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        // parse line
        char* key = strtok(line, " ");
        char* value = strtok(NULL, "\n");
        // create instruction
        instruction_t* instruction = (instruction_t*) malloc(sizeof(instruction_t));
        instruction->name = malloc(sizeof(char) * (strlen(key) + 1));
        strcpy(instruction->name, key);
        instruction->time = atoi(value);
        // add instruction to hash table
        put(instructions, key, instruction);
    }
    fclose(file);
}

int read_definitions(char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File %s not found\n", filename);
        exit(1);
    }
    int num_processes = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        // parse line
        char* name = strtok(line, " ");
        char* priority = strtok(NULL, " ");
        char* arrival_time = strtok(NULL, " ");
        char* type = strtok(NULL, "\n");
        // create process
        process_t* process = (process_t*) malloc(sizeof(process_t));
        // printf("%d\n", process);
        process->name = malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(process->name, name);
        // read program
        char program_name[20];
        sprintf(program_name, "%s.txt", name);
        process->program = read_program(program_name);
        // initialize attributes
        process->initial_time = atoi(arrival_time);
        process->arrival_time = process->initial_time;
        process->priority = atoi(priority);
        process->burst_time = 0;
        process->finish_time = 0;
        process->quantum_counter = 0;
        // initialize type
        if (strcmp(type, "SILVER") == 0) {
            process->type = SILVER;
        } else if (strcmp(type, "GOLD") == 0) {
            process->type = GOLD;
        } else if (strcmp(type, "PLATINUM") == 0) {
            process->type = PLATINUM;
        } else {
            printf("Error: Invalid process type %s\n", type);
            exit(1);
        }
        // add process to wait queue
        push(wait_queue, process);
        num_processes++;
    }
    fclose(file);
    return num_processes;
}

struct program_t* read_program(char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File %s not found\n", filename);
        exit(1);
    }
    // define program and helper pointers
    struct program_t* head = malloc(sizeof(struct program_t));
    struct program_t* program = head;
    struct program_t* last = NULL;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char* key = strtok(line, "\n");
        // get instruction from hash table
        instruction_t* instruction = (instruction_t*) get(instructions, key);
        if (instruction == NULL) {
            printf("Error: Instruction %s not found\n", key);
            exit(1);
        }
        // add instruction to program linked list
        program->instruction = instruction;
        program->next = malloc(sizeof(struct program_t));
        last = program;
        program = program->next;
    }
    // free last program node
    free(program);
    last->next = NULL;
    fclose(file);
    return head;
}

void exit_scheduler(){
    free_hash(instructions);
    free_heap(ready_queue);
    free_heap(wait_queue);
}

int ready_queue_compare(void* a, void* b){ // compare processes in ready queue
    process_t* process_a = (process_t*) a;
    process_t* process_b = (process_t*) b;
    // compare processes by type, priority, arrival time, and name
    if (process_a->type == PLATINUM && process_b->type != PLATINUM)
        return -1;
    if (process_a->type != PLATINUM && process_b->type == PLATINUM)
        return 1;
    // if both processes are platinum or both are not platinum, compare by priority
    if (process_a->priority != process_b->priority)
        return process_a->priority - process_b->priority;
    // if both processes have the same priority, compare by arrival time
    if (process_a->arrival_time != process_b->arrival_time)
        return process_a->arrival_time - process_b->arrival_time;
    // if both processes have the same arrival time, compare by name
    return strcmp(process_a->name, process_b->name);
}

int wait_queue_compare(void* a, void* b){ // compare processes in wait queue
    process_t* process_a = (process_t*) a;
    process_t* process_b = (process_t*) b;
    // compare processes by arrival time and name
    if (process_a->arrival_time == process_b->arrival_time)
        return strcmp(process_a->name, process_b->name);
    return process_a->arrival_time - process_b->arrival_time;
}
