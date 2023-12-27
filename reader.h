#ifndef READER_H
#define READER_H

#include "types.h"

int init_scheduler(char* instructions_file, char* program_file);
void read_instructions(char *filename);
int read_definitions(char *filename);
struct program_t* read_program(char *filename);
void exit_scheduler();
int ready_queue_compare(void* a, void* b);
int wait_queue_compare(void* a, void* b);

#endif