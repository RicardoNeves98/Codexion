#include "codexion.h"

shared_data *init_data(int *parsed_args)
{
    int coder_num;
    shared_data *data;

    data = malloc(sizeof(*data));
    if (!data)
        return (NULL);
    data->coder_num = parsed_args[0];
    data->time_to_burnout = parsed_args[1];
    data->time_to_compile = parsed_args[2];
    data->time_to_debug = parsed_args[3];
    data->time_to_refactor = parsed_args[4];
    data->number_of_compiles_required = parsed_args[5];
    data->dongle_cooldown = parsed_args[6];
    data->number_of_compiles = get_zeros_arr(coder_num);
    data->number_of_dongles = get_zeros_arr(coder_num);
    return (data);
}

queue *init_queue(int coder_num)
{
    int i;
    struct queue *start_queue;
    struct queue *curr_queue;
    struct queue *next_queue;

    i = 0;
    start_queue = malloc(sizeof(*start_queue));
    start_queue->id = 0;
    start_queue->next = NULL;
    curr_queue = start_queue;
    while (++i < coder_num)
    {
        next_queue = malloc(sizeof(*next_queue));
        next_queue->id = i;
        next_queue->next = NULL;
        curr_queue->next = next_queue;
        curr_queue = next_queue;
    }
    return (start_queue);
}

dongle *init_dongle_list(int coder_num)
{
    int i;
    struct dongle *dongle_list;

    i = -1;
    dongle_list = malloc(coder_num * sizeof(*dongle_list));
    while (++i < coder_num)
    {
        dongle_list[i].last_used = 0;
        pthread_mutex_init(&dongle_list[i].dongle_lock);
    }
    return (dongle_list);
}

manager *init_manager(void)
{
    struct *manager;

    manager = malloc(sizeof(*manager));
    pthread_mutex_init(&manager->queue_lock, NULL);
    pthread_cond_init(&manager->check_next, NULL);
    return (manager);
}
