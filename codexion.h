#ifndef CODEXION_H
# define CODEXION_H
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

typedef struct shared_data
{
    int coder_num;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int comp_required;
    int cooldown;
    int scheduler;
    int coders_active;
    struct timespec start_burnout;
    struct timespec comp_burnout;
    struct timespec max_wait;
    struct timespec start_time;
    struct queue *deadline;
    struct dongle *dongles;
    pthread_mutex_t output_mutex;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;
}   shared_data;

typedef struct queue
{
    int id;
    struct timespec time;
}   queue;

typedef struct dongle
{
    int id;
    int is_free;
    struct timespec cooldown;
    struct timespec next_aval;
    struct queue *requests;
    pthread_mutex_t dongle_mutex;
    pthread_cond_t dongle_cond;
}   dongle;

typedef struct coders_state
{
    int id;
    int num_compiles;
    struct timespec last_compile;
    struct dongle *left_dongle;
    struct dongle *right_dongle;
    struct shared_data *data;
}   coder_thread;

typedef struct monitor_state
{
    int id;
    int total_required;
    struct shared_data *data;
}   monitor_thread;

// compile.c
void update_deadline_queue(struct coders_state *coder_info, int compile);
void update_dongle_state(struct dongle *curr_dongle);
void write_output(char *type, struct coders_state *coder_info);
void go_work(struct coders_state *coder_info);

// free_stuff.c
void free_dongles(struct dongle *dongles, int coder_num);
void free_all(struct coders_state *coders_info, struct monitor_state *monitor_info,
              pthread_t *threads);

// func_coder.c
void make_request(struct dongle *left_dongle, struct dongle *right_dongle,
                  int id, int scheduler, struct timespec time);
void delete_requests(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, int has_dongle);
int check_queues(struct dongle *left_dongle, struct dongle *right_dongle,
                  int coder_id);
void get_both_dongles(struct coders_state *coder_info);
int check_active(struct coders_state *coder_info);
void *coder_func(void *info);

// func_monitor.c
void signal_dongles(struct monitor_state *monitor_info);
void *finish(struct monitor_state *monitor_info, int coder_burned);
void *monitor_func(void *info);

// init_data.c 
void fill_data(int *parsed_args, struct shared_data *data);
int init_data_mutex_and_cond(struct shared_data *data);
struct shared_data *init_data(int *parsed_args);

// init_threads.c
int init_dongle_mutex_and_cond(struct dongle *curr_dongle);
struct dongle *create_dongles(int coder_num, struct timespec cooldown);
struct coders_state *init_coders_state(struct shared_data *data);
struct monitor_state *init_monitor_state(struct shared_data *data);
int init_threads(pthread_t *threads,
                 struct coders_state *coder_info, void *(*coder_func)(void *),
                 struct monitor_state *monitor_info, void *(*monitor_func)(void *));

// get_dongle.c
int check_aval(struct dongle *curr_dongle, int coder_id);
int wait_aval(struct dongle *curr_dongle, struct timespec time_limit, int coder_id);
int get_dongle(struct coders_state *coder_info, struct dongle *curr_dongle,
               struct timespec time_limit);

// parsing.c
int *display_error(char *inv_arg, int i);
int *parsing(int argc, char **argv);

// queue_deadline.c
struct queue *init_deadline(int coder_num, struct timespec start_burnout);
void move_around(struct queue *deadline, int start, int end);
void first_deadline_update(struct queue *deadline, int coder_num,
                           struct timespec burnout);
void comp_deadline_update(struct queue *deadline, int coder_num, int coder_id,
                          struct timespec burnout, int finished);
void print_deadline(struct queue *deadline, int coder_num);

// queue_requests.c
struct queue *init_requests(void);
void insert_fifo(struct queue *requests, int id);
void insert_edf(struct queue *requests, int id, struct timespec last_compile);
void update_requests(struct queue *requests);
int remove_requests(struct queue *requests, int id);

// queue_utils.c
void switch_spots(struct queue *line, int index1, int index2);

// time_conversion.c
struct timespec ms_to_timespec(int time_ms);
long timespec_to_ms(struct timespec time);

// time_utils.c
long get_time_diff(struct timespec time1, struct timespec time2);
struct timespec get_min_time(struct timespec time1, struct timespec time2);
struct timespec add_time(struct timespec time1, struct timespec time2);
struct timespec add_curr_time(struct timespec time);

