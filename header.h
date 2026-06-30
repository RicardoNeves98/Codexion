#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <stdlib.h>

typedef struct thread_state
{
    int id;
    struct shared_config *config;
    struct request_queue *queue;
}

typedef struct shared_config
{
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    int scheduler;
    int curr_coder;
    int *number_of_compiles;
}   shared_config;

typedef struct request_queue
{
    int id;
    struct request_queue *next;
}   request_queue;

