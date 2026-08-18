#include "codexion.h"

struct dongle *create_dongles(int coder_num, struct timespec cooldown)
{
    int i;
    struct dongle *dongles;
    struct timespec now;

    i = -1;
    dongles = malloc(coder_num * sizeof(*dongles));
    if (!dongles)
        return (printf("Error allocation dongles\n"), NULL);
    while (++i < coder_num)
    {
        clock_gettime(CLOCK_MONOTONIC, &now);
        dongles[i].next_aval = now;
        dongles[i].is_free = 1;
        dongles[i].cooldown = cooldown;
        dongles[i].requests = init_requests();
        if (!dongles[i].requests)
            return (NULL);
        pthread_mutex_init(&dongles[i].next, NULL);
        pthread_mutex_init(&dongles[i].state, NULL);
        pthread_cond_init(&dongles[i].available, NULL);
    }
    return (dongles);
}

struct coders_state *init_coders_state(int cooldown, struct shared_data *data)
{
    int i;
    struct dongle *dongles;
    struct coder_thread *coders_info;

    i = -1;
    dongles = create_dongles(coder_num, ms_to_timespec(data->cooldown));
    data->dongles = dongles;
    coders_info = malloc(coder_num * sizeof(*coders_info));
    if (!coders_info)
        return (printf("Error allocation coders data\n"), NULL);
    while (++i < data->coder_num)
    {
        coders_info[i].id = i + 1;
        coders_info[i].num_compiles = 0;
        coders_info[i].data = data;
        coders_info[i].left_dongle = &dongles[i];
        coders_info[i].right_dongle = &dongles[(i + 1) % coder_num];
    }
    return (coders_info);
}

struct monitor_state *init_monitor_state(struct shared_data *data)
{
    struct monitor_thread *monitor_info;

    monitor_info = malloc(sizeof(*monitor_info));
    if (!monitor_info)
        return (printf("Error allocation monitor data\n"), NULL);
    monitor_info->id = 0;
    monitor_info->total_comp = 0;
    monitor_info->total_required = data->coder_num * data->compiles_required;
    monitor_info->data = data;
    return (monitor_info);
}

int init_threads(struct coder_thread *coder_info, void *(*coder_func)(void *),
                 struct monitor_thread *monitor_info, void *(*monitor_func)(void *))
{
    int i;

    i = 0;
    if (pthread_create(&threads[0], NULL, monitor_func, monitor_info))
        return (printf("Error creating monitor thread\n"), 0);
    while (++i <= coder_info->data->coder_num)
    {
        if (pthread_create(&threads[i], NULL, coder_func, &coders_info[i]))
            return (printf("Error creating coder thread\n"), 0);
        update_deadline(data->deadline, i + 1, &data->time_to_burnout;
    }
    return (1);
}
