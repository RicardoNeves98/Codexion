#ifndef CODEXION_H
# define CODEXION_H
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

typedef struct coders_state
{
    int id;
    int num_compiles;
    int *retry_num;
    struct timespec *last_compile;
    struct dongle *left_dongle;
    struct dongle *right_dongle;
    struct shared_data *data;
}   coder_thread;

typedef struct shared_data
{
    int coder_num;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int comp_required;
    int cooldown;
    int coders_active;
    struct timespec start_burnout;
    struct timespec comp_burnout;
    struct timespec max_wait;
    struct timespec start_time;
    void (*place_request)(int *line, struct coders_state *coder_info);
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
    int *line;
    struct timespec cooldown;
    struct timespec next_aval;
    pthread_mutex_t dongle_mutex;
    pthread_cond_t dongle_cond;
}   dongle;

typedef struct monitor_state
{
    int id;
    int total_required;
    struct shared_data *data;
}   monitor_thread;

// compile.c
void update_deadline_queue(struct coders_state *coder_info, int compile);
void update_dongle_state(struct dongle *curr_dongle);
int check_coders(struct coders_state *coder_info);
void write_output(char *type, struct coders_state *coder_info);
void go_work(struct coders_state *coder_info);

// deadline_queue.c
struct queue *init_deadline(int coder_num, struct timespec start_burnout);
void move_around(struct queue *deadline, int start, int end);
void first_deadline_update(struct queue *deadline, int coder_num,
                           struct timespec burnout);
void comp_deadline_update(struct queue *deadline, int coder_num, int coder_id,
                          struct timespec burnout, int finished);
void print_deadline(struct queue *deadline, int coder_num);

// free_stuff.c
void free_dongles(struct dongle *dongles, int coder_num);
void free_shared_data(struct shared_data *data);
void free_all(struct coders_state *coders_info, struct monitor_state *monitor_info,
              pthread_t *threads);

// func_coder.c
void make_request(struct dongle *left_dongle, struct dongle *right_dongle,
                  struct coders_state *coder_info);
void delete_requests(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, int has_dongle);
int check_queues(struct dongle *left_dongle, struct dongle *right_dongle,
                  int coder_id);
void get_both_dongles(struct dongle *left_dongle, struct dongle *right_dongle,
                      struct coders_state *coder_info);
int check_active(struct coders_state *coder_info);
void *coder_func(void *info);

// func_monitor.c
void signal_dongles(struct monitor_state *monitor_info);
void *finish(struct monitor_state *monitor_info, int coder_burned);
void *monitor_func(void *info);

// get_dongle.c
int check_aval(struct dongle *curr_dongle, int coder_id);
int wait_aval(struct dongle *curr_dongle, struct timespec time_limit, int coder_id);
int get_dongle(struct coders_state *coder_info, struct dongle *curr_dongle,
               struct timespec time_limit);

// init_data.c 
void fill_data(int *parsed_args, struct shared_data *data);
int init_data_mutex_and_cond(struct shared_data *data);
struct shared_data *init_data(int *parsed_args);

// init_threads.c
int init_threads(pthread_t *threads, struct coders_state *coders_info,
                 void *(*coder_func)(void *), struct monitor_state *monitor_info,
                 void *(*monitor_func)(void *));

// init_threads_state.c
int init_dongle_mutex_and_cond(struct dongle *curr_dongle);
struct dongle *create_dongles(int coder_num, struct timespec cooldown);
int get_shared_arrays(int coder_num, struct coders_state *coders_info);
struct coders_state *init_coders_state(struct shared_data *data);
struct monitor_state *init_monitor_state(struct shared_data *data);

// line_requests.c
void place_fifo_request(int *line, struct coders_state *coder_info);
void place_edf_request(int *line, struct coders_state *coder_info);
void update_line(int *line);
int remove_requests(int *line, int id);
void print_line(char *text, int *line, int id);

// parsing.c
int *display_error(char *inv_arg, int i);
int *parsing(int argc, char **argv);

// time_conversion.c
struct timespec ms_to_timespec(int time_ms);
long timespec_to_ms(struct timespec time);

// time_utils.c
long get_time_diff(struct timespec time1, struct timespec time2);
struct timespec get_min_time(struct timespec time1, struct timespec time2);
struct timespec add_time(struct timespec time1, struct timespec time2);
struct timespec add_curr_time(struct timespec time);

