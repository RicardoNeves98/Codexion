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
    return (data);
}

dongle *init_dongle_list(int coder_num)
{
    int i;
    struct dongle *dongle_list;

    i = -1;
    dongle_list = malloc(coder_num * sizeof(*dongle_list));
    while (++i < coder_num)
    {
        dongle_list[i].is_free = 1;
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
