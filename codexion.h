#ifndef CODEXION_H
# define CODEXION_H
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct timespec
{
    time_t tv_sec;
    long tv_nsec;
}   timespec;

typedef struct shared_data
{
    int coder_num;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int compiles_required;
    int scheduler;
    struct timespec max_wait;
    int *number_of_compiles;
    pthread_mutex_t output;
}   shared_data;

typedef struct queue
{
    int id;
    struct timespec last_compile;
    struct queue *next;
}   queue;

typedef struct dongle
{
    int is_free;
    struct timespec cooldown;
    struct timespec next_aval;
    struct *queue;
    pthread_mutex_t next;
    pthread_mutex_t state;
    pthread_cond_t available;
}   dongle;

typedef struct coder_thread
{
    int id;
    struct timespec last_compile;
    struct dongle *left_dongle;
    struct dongle *right_dongle;
    struct shared_data *data;
}   coder_thread;

typedef struct monitor_thread
{
    int id;
    struct shared_data *data;
}   monitor_thread;

// coder_func.c
void go_work(struct coder_thread *coder_info, struct dongle *dongle1,
             struct dongle *dongle2);
int check_aval(struct dongle *curr_dongle);
void make_request(struct dongle *dongle1, struct dongle *dongle2, int scheduler,
                  int id, struct timespec time);
int get_dongle(struct dongle *curr_dongle, int coder_id, int max_wait);
int check_next(struct dongle *dongle1, struct dongle *dongle2, int coder_id);
void *coder_func(void *coder_state);

// init_threads.c
struct shared_state *init_state(void);
int create_coder_thread(pthread_t *threads, int coder_num, int cooldown,
                        shared_data *data, void *(*coder_func)(void*));
int init_threads(pthread_t *threads, int *parsed_args,
                 void *(*coder_func)(void *), void *(*monitor_func)(void *));

// monitor_func.c

// parsing.c
int *display_error(char *inv_arg, int i);
int *parsing(char **argv);

// queue_utils.c
void update_queue(struct dongle *curr_dongle);
void remove_request(struct dongle *curr_dongle, int coder_id);
void remove_requests(struct dongle *dongle1, struct dongle *dongle2, int coder_id);

// scheduler.c 
void fifo(struct queue **queue_head, int id);
void edf(struct queue **queue_head, int id, struct timespec time);

// struct_utils.c 
shared_data *init_data(int *parsed_args);
dongle *init_dongle_list(int coder_num, int cooldown);

// time_utils.c
struct timespec time_convert(int time_ms);
int cmp_time(struct timespec time1, struct timespec time2);
struct timespec add_time(struct timespec time1, struct timespec time2);
void update_next_aval(dongle *curr_dongle);

// utils.c 
int *get_zeros_arr(int size);
void switch_dongles(struct dongle **curr_dongle, struct dongle **next_dongle);
