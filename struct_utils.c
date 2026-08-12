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
    data->compiles_required = parsed_args[5];
    data->scheduler = parsed_args[7];
    data->max_wait = time_convert(parsed_args[2] + parsed_args[6]);
    data->number_of_compiles = get_zeros_arr(coder_num);
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
        dongle_list[i].queue = NULL;
        pthread_mutex_init(&dongle_list[i].next, NULL);
        pthread_mutex_init(&dongle_list[i].state, NULL);
        pthread_cond_init(&dongle_list[i].available, NULL);
    }
    return (dongle_list);
}
