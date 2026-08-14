#include "codexion.h"

shared_data *init_data(int *parsed_args)
{
    int coder_num;
    struct timespec now;
    shared_data *data;

    data = malloc(sizeof(*data));
    if (!data)
        return (NULL);
    clock_gettime(CLOCK_MONOTONIC, &now);
    data->coder_num = parsed_args[0];
    data->time_to_compile = parsed_args[2];
    data->time_to_debug = parsed_args[3];
    data->time_to_refactor = parsed_args[4];
    data->compiles_required = parsed_args[5];
    data->scheduler = parsed_args[7];
    data->time_to_burnout = time_convert(parsed_args[1]);
    data->max_wait = ms_to_timespec(parsed_args[2] + parsed_args[6]);
    data->start_time = now;
    data->deadline = NULL;
    pthread_mutex_init(&data->output);
    pthread_mutex_init(&data->state);
    pthread_cond_init(&data->cond);
    return (free(parsed_args), data);
}

dongle *init_dongle_list(int coder_num, int cooldown)
{
    int i;
    struct dongle *dongle_list;
    struct timespec now;
    struct timespec cooldown_timespec;

    i = -1;
    dongle_list = malloc(coder_num * sizeof(*dongle_list));
    clock_gettime(CLOCK_MONOTONIC, &now);
    cooldown_tv = time_convert(cooldown);
    while (++i < coder_num)
    {
        dongle_list[i].is_free = 1;
        dongle_list[i].cooldown = cooldown_tv;
        dongle_list[i].next_aval = now;
        dongle_list[i].next_to_use = NULL;
        pthread_mutex_init(&dongle_list[i].next, NULL);
        pthread_mutex_init(&dongle_list[i].state, NULL);
        pthread_cond_init(&dongle_list[i].available, NULL);
    }
    return (dongle_list);
}
