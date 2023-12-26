#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "hash.h"
#include "heap.h"

// some macros for readability
#define IS_NOT_EMPTY(x) (x->heap_size > 0)
#define IS_PLATINUM(x) (x->type == PLATINUM)
#define INSTRUCTION_TIME(x) (x->program->instruction->time)
#define TURNAROUND_TIME(x) (x->finish_time - x->initial_time)
#define WAITING_TIME(x) (x->finish_time - x->burst_time - x->initial_time)
#define CONTEXT_SWITCH 10
#define QUANTUM(x) (x->type == SILVER ? 80 : 120)
#define EQUALS(x, y) (strcmp(x->name, y) == 0)
#define FINISHED(x) (x->program == NULL)
#define CAN_UPGRADE(x) (x->type == SILVER ? x->quantum_counter >= 3 : x->quantum_counter >= 5)
#define UPGRADE(x) (x->type + 1)
#define FREE(x) (free(x->name), free(x))
#define PRINTF_DIV(x, y) (x % y == 0 ? printf("%d\n", x / y) : printf("%.1f\n", (float) x / y))

extern hash_table_t* instructions;
extern heap_t* ready_queue;
extern heap_t* wait_queue;

int main(int argc, char** argv){
    int num_processes = init_scheduler("instructions.txt", argv[1]);
    int global_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    char* last_process = NULL;
    while (IS_NOT_EMPTY(wait_queue) || IS_NOT_EMPTY(ready_queue)) { // loop until both queues are empty
        // check processes in the wait queue that should enter the ready queue
        while (IS_NOT_EMPTY(wait_queue) && ((process_t*) seek(wait_queue))->arrival_time <= global_time)
            push(ready_queue, poll(wait_queue));
        // take first process in the ready queue
        if (IS_NOT_EMPTY(ready_queue)) {
            process_t* process = (process_t*) poll(ready_queue);
            struct program_t* program = process->program;
            // fprintf(stderr, "%s %d\n", process->name, global_time);
            // if (last_process != NULL)
                // printf("%s %d %d %d\n", last_process->name, global_time, last_process, process);
            if (last_process == NULL || !EQUALS(process, last_process)){ // context switch
                global_time += CONTEXT_SWITCH;
            }
            if (IS_PLATINUM(process)) { // no preemption
                while (program != NULL) {
                    global_time += INSTRUCTION_TIME(process);
                    process->burst_time += INSTRUCTION_TIME(process);
                    process->program = program->next;
                    free(program);
                    program = process->program;
                }
                process->finish_time = global_time;
                total_turnaround_time += TURNAROUND_TIME(process);
                total_waiting_time += WAITING_TIME(process);
                last_process = NULL;
                FREE(process);
            } else { // can be preempted
                global_time += INSTRUCTION_TIME(process);
                process->burst_time += INSTRUCTION_TIME(process);
                process->quantum_time += INSTRUCTION_TIME(process);
                process->program = program->next;
                free(program);
                int quantum = QUANTUM(process);
                if (process->quantum_time >= quantum) { // check for upgrade
                    process->quantum_counter++;
                    process->quantum_time = 0;
                    if (CAN_UPGRADE(process)) {
                        process->type = UPGRADE(process);
                        process->quantum_counter = 0;
                    }
                    process->arrival_time = global_time; // for round robin
                }
                last_process = process->name;
                // printf("ananin ami galatasaray %s %d %d %d\n", last_process->name, global_time, last_process, process);
                if (FINISHED(process)) { // process finished
                    process->finish_time = global_time;
                    total_turnaround_time += TURNAROUND_TIME(process);
                    total_waiting_time += WAITING_TIME(process);
                    last_process = NULL;
                    FREE(process);
                } else // process not finished
                    push(ready_queue, process);
            }
        } else // no process in the ready queue
            global_time = ((process_t*) seek(wait_queue))->arrival_time;
    }
    exit_scheduler();
    PRINTF_DIV(total_waiting_time, num_processes);
    PRINTF_DIV(total_turnaround_time, num_processes);
    return 0;
}
