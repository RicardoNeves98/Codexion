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
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int comp_required;
    int cooldown;
    int scheduler;
    struct timespec time_to_burnout;
    struct timespec max_wait;
    struct timespec start_time;
    struct queue *deadline;
    struct dongle *dongles;
    pthread_mutex_t output;
    pthread_mutex_t state;
    pthread_cond_t cond;
}   shared_data;

typedef struct queue
{
    int id;
    struct timespec *time;
}   queue;

typedef struct dongle
{
    int is_free;
    struct timespec cooldown;
    struct timespec next_aval;
    struct queue *requests;
    pthread_mutex_t next;
    pthread_mutex_t state;
    pthread_cond_t available;
}   dongle;

typedef struct coder_thread
{
    int id;
    int num_compiles;
    struct timespec last_compile;
    struct dongle *left_dongle;
    struct dongle *right_dongle;
    struct shared_data *data;
}   coder_thread;

typedef struct monitor_thread
{
    int id;
    int total_comp;
    int total_required;
    struct shared_data *data;
}   monitor_thread;

// compile.c
void update_deadline_queue(struct coder_thread *coder_info);
void update_dongle_state(struct dongle *curr_dongle);
void write_output(char *type, struct coder_thread *coder_info);
void go_work(struct coder_thread *coder_info);

// free_stuff.c
void free_dongles(struct dongle *dongles, int coder_num);
void free_all(struct coders_state *coders_info, struct monitor_state *monitor_info,
              pthread_t *threads);

// func_coder.c
void make_request(struct dongle *dongle1, struct dongle *dongle2,
                  int id, int scheduler, struct timespec time);
void remove_requests(struct dongle *dongle1, struct dongle *dongle2, int coder_id);
void start_over(struct dongle *dongle1, struct dongle *dongle2, int coder_id);
int get_both_dongles(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, struct timespec time_limit);
void *coder_func(void *coder_state);

// func_monitor.c
void *monitor_func(void *monitor_state);

// init_data.c 
void fill_data(int *parsed_args, struct shared_data *data);
int init_data_mutex_and_cond(struct shared_data *data);
struct shared_data *init_data(int *parsed_args);

// init_threads.c
struct shared_state *init_state(void);
int create_coder_thread(pthread_t *threads, int coder_num, int cooldown,
                        shared_data *data, void *(*coder_func)(void*));
int init_threads(pthread_t *threads, int *parsed_args,
                 void *(*coder_func)(void *), void *(*monitor_func)(void *));

// get_dongle.c
int check_aval(struct dongle *curr_dongle);
int wait_line(struct dongle *curr_dongle, int coder_id, struct timespec time_limit);
int wait_aval(struct dongle *curr_dongle, int coder_id, struct timespec time_limit);
int get_dongle(struct dongle *curr_dongle, int coder_id, struct timespec time_limit);

// parsing.c
int *display_error(char *inv_arg, int i);
int *parsing(char **argv);

// queue_deadline.c
struct queue *init_deadline(int coder_num);
void update_deadline(struct queue *deadline, int coder_num,
                     int id, struct timespec *burnout);

// queue_requests.c
struct queue *init_requests(void);
void insert_fifo(struct queue *requests, int id);
void insert_edf(struct queue *requests, int id, struct timespec *last_compile);
void update_requests(struct queue *requests);
int remove_requests(struct queue *requests, int id);

// queue_utils.c
void switch_spots(struct queue *line, int index1, int index2);

// time_conversion.c
struct timespec ms_to_timespec(int time_ms);
int timespec_to_ms(struct timespec time);

// time_utils.c
int get_time_diff(struct timespec time1, struct timespec time2);
struct timespec get_min_time(struct timespec time1, struct timespec time2);
struct timespec add_time(struct timespec time1, struct timespec time2);
struct timespec add_curr_time(struct timespec time);

