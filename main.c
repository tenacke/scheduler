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
#define EQUALS(x, y) (strcmp(x->name, y->name) == 0)
#define FINISHED(x) (x->program == NULL)
#define CAN_PROMOTE(x) (x->quantum_time >= (x->type == SILVER ? 80 : 120))
#define CAN_PROMOTE_PREEMPTED(x) (x->quantum_time > 0)
#define CAN_UPGRADE(x) (x->type == SILVER ? x->quantum_counter >= 3 : x->quantum_counter >= 5)
#define UPGRADE(x) (x->type += 1)
#define FREE(x) (free(x->name), free(x))
#define PRINTF_DIV(x, y) (x % y == 0 ? printf("%d\n", x / y) : printf("%.1f\n", (float) x / y))

extern hash_table_t* instructions;
extern heap_t* ready_queue;
extern heap_t* wait_queue;

int step_program(process_t* process);
void update_queue(int global_time);
void promote(process_t* process, int global_time);

int main(int argc, char** argv){
    int num_processes = init_scheduler("instructions.txt", argv[1]);
    int global_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    process_t* last_process = NULL;
    update_queue(global_time);
    while (IS_NOT_EMPTY(wait_queue) || IS_NOT_EMPTY(ready_queue)) { // loop until both queues are empty
        // take first process in the ready queue
        if (IS_NOT_EMPTY(ready_queue)) {
            process_t* process = (process_t*) poll(ready_queue);
            if (last_process == NULL || !EQUALS(process, last_process)) { // last process is not the same as current process
                if (last_process != NULL && !FINISHED(last_process)) { // preempted
                    if (CAN_PROMOTE_PREEMPTED(last_process)) { // reset quantum time
                        promote(last_process, global_time);
                        relocate(ready_queue, last_process);
                    }
                    push(ready_queue, process);
                    process = (process_t*) poll(ready_queue);
                }
                // fprintf(stderr, "Context switch %d\n", global_time);
                if (last_process == NULL || !EQUALS(process, last_process)) // really context switch
                    global_time += CONTEXT_SWITCH;
            }
            last_process = process;
            if (IS_PLATINUM(process)) { // no preemption
                // fprintf(stderr, "PLATINUM %s %d\n", process->name, global_time);
                while (process->program != NULL) {
                    global_time += step_program(process);
                }
            } else { // can be preempted
                // fprintf(stderr, "Execution %s %d\n", process->name, global_time);
                global_time += step_program(process);
                if (CAN_PROMOTE(process)) { // check for upgrade
                    promote(process, global_time);
                }
            }
            update_queue(global_time);
            if (FINISHED(process)) { // when process finished
                // fprintf(stderr, "Finish %s %d\n", process->name, global_time);
                process->finish_time = global_time;
                total_turnaround_time += TURNAROUND_TIME(process);
                total_waiting_time += WAITING_TIME(process);
                FREE(process);
            } else //if (IS_NOT_PREEMPTEE(process)) // process not finished and will not be preempted
                push(ready_queue, process);
        } else { // no process in the ready queue
            global_time = ((process_t*) seek(wait_queue))->arrival_time;
            update_queue(global_time);
        } 
    }
    exit_scheduler();
    PRINTF_DIV(total_waiting_time, num_processes);
    PRINTF_DIV(total_turnaround_time, num_processes);
    return 0;
}

int step_program(process_t* process) {
    int time = INSTRUCTION_TIME(process);
    struct program_t* program = process->program;
    process->burst_time += time;
    process->quantum_time += time;
    process->program = program->next;
    free(program);
    return time;
}

void update_queue(int global_time){
    while (IS_NOT_EMPTY(wait_queue) && ((process_t*) seek(wait_queue))->arrival_time <= global_time)
        push(ready_queue, poll(wait_queue));
}

void promote(process_t* process, int global_time) {
    process->quantum_counter++;
    process->quantum_time = 0;
    if (CAN_UPGRADE(process)) {
        UPGRADE(process);
        process->quantum_counter = 0;
    }
    process->arrival_time = global_time; // for round robin
}
