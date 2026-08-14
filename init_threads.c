#include "codexion.h"

shared_state *init_state(void)
{
    shared_state *state;

    state = malloc(sizeof(*state));
    if (!state)
        return (NULL);
    pthread_mutex_init(&state->wait, NULL);
    pthread_cond_init(&state->available, NULL);
    return (state);
}   

int create_coder_thread(pthread_t *threads, int coder_num, int cooldown,
                        shared_data *data, void *(*coder_func)(void*))
{
    int i;
    struct coder_thread *coder_info;
    struct dongle *dongles;
    struct shared_state *state;

    i = 0;
    dongles = init_dongle_list(coder_num, cooldown);
    if (!dongles)
        return (printf("Error\n"), 0);
    state = init_state();
    if (!state)
        return (printf("Error\n"), 0);
    while (++i <= coder_num)
    {
        coder_info = malloc(sizeof(*coder_info));
        if (!coder_info)
            return (printf("Error\n"), 0);
        coder_info->id = i;
        coder_info->num_compiles = 0;
        coder_info->left_dongle = &dongles[i - 1];
        coder_info->right_dongle = &dongles[i % coder_num];
        coder_info->data = data;
        coder_info->state = state;
        if (pthread_create(&threads[i], NULL, coder_func, coder_info))
            return (printf("Error\n"), 0);
        if (!insert_deadline(&data->deadline, data->time_to_burnout, i))
            return (printf("Error\n"), 0);
    }
    return (1);
}

int init_threads(pthread_t *threads, int *parsed_args,
                 void *(*coder_func)(void *), void *(*monitor_func)(void *))
{
    int coder_num;
    int cooldown;
    shared_data *data;
    monitor_thread *monitor_info;

    coder_num = parsed_args[0];
    cooldown = parsed_args[6];
    total_required = parsed_args[0] * parsed_args[5];
    data = init_data(parsed_args);
    if (!data)
        return (printf("Error\n"), 0);
    if (!create_coder_thread(threads, coder_num, cooldown, data, coder_func))
        return (printf("Error\n"), 0);
    monitor_info = malloc(sizeof(*monitor_info));
    if (!monitor_info)
        return (printf("Error\n"), 0);
    monitor_info->id = 0;
    monitor_info->total_comp = 0;
    monitor_info->total_required = total_required;
    monitor_info->data = data;
    if (pthread_create(&threads[0], NULL, monitor_func, monitor_info))
        return (printf("Error\n"), 0);
    return (1);
}
