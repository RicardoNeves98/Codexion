#ifndef CODEXION_H
# define CODEXION_H
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct shared_data
{
    int coder_num;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int time_to_cooldown;
    int *number_of_compiles;
    int *number_of_dongles;
}   shared_data;

typedef struct manager
{
    pthread_mutex_t lock;
    pthread_cond_wait cond; 
}   manager;

typedef struct coder_thread
{
    int thread_id;
    struct shared_data *data;
    struct queue *coder_queue;
    struct dongle_list *dongles;
    struct manager *state;
}   coder_thread;

typedef struct monitor_thread
{
    int thread_id;
    struct shared_data *data;
}   monitor_thread;

typedef struct dongle
{
    int free_dongle;
    int last_used;
    pthread_mutex_t dongle_lock;
}   dongle;

typedef struct queue
{
    int id;
    struct queue *next;
}   queue;

// parsing.c
int *display_error(char *inv_arg, int i);
int *parsing(char **argv);

// init_threads.c
void *coder_func(void *coder_state);
void *monitor_func(void *monitor_state);
int create_coder_thread(pthread_t *threads, int coder_num, shared_data *data, 
                        void *(*coder_func)(void *));
int init_threads(pthreads, int *parsed_args,
                 void *(*coder_func)(void *), void *(*monitor_func)(void *));

// struct_utils.c 
shared_data *init_data(int *parsed_args);
queue *init_queue(int coder_num);
dongle *init_dongle_list(int coder_num);
manager *init_manager(void);

// utils.c 
int *get_zeros_arr(int size);
int make_request(int coder_id, int dongle_num, coder_thread *coder_info);
queue *new_request(int coder_id);
int give_dongles(coder_thread *coder_info);
void give_dongle(int dongle_id, coder_thread *coder_info);
