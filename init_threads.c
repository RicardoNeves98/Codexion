#include "codexion.h"

int init_dongle_mutex_and_cond(struct dongle *curr_dongle)
{
    if (pthread_mutex_init(&curr_dongle->dongle_mutex, NULL))
    {
        printf("Error initializing mutex variable\n");
        return (0);
    }
    if (pthread_cond_init(&curr_dongle->dongle_cond, NULL))
    {
        printf("Error initializing mutex variable\n");
        pthread_mutex_destroy(&curr_dongle->dongle_mutex);
        return (0);
    }
    return (1);
}

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
        clock_gettime(CLOCK_REALTIME, &now);
        dongles[i].next_aval = now;
        dongles[i].id = i + 1;
        dongles[i].is_free = 1;
        dongles[i].cooldown = cooldown;
        dongles[i].requests = init_requests();
        if (!dongles[i].requests)
            return (free_dongles(dongles, i), NULL);
        if (!init_dongle_mutex_and_cond(&dongles[i]))
        {
            free(dongles[i].requests);
            return (free_dongles(dongles, i), NULL);
        }
    }
    return (dongles);
}

struct coders_state *init_coders_state(struct shared_data *data)
{
    int i;
    struct dongle *dongles;
    struct coders_state *coders_info;

    i = -1;
    dongles = create_dongles(data->coder_num, ms_to_timespec(data->cooldown));
    if (!dongles)
        return (NULL);
    data->dongles = dongles;
    coders_info = malloc(data->coder_num * sizeof(*coders_info));
    if (!coders_info)
        return (printf("Error allocation coders data\n"), NULL);
    while (++i < data->coder_num)
    {
        coders_info[i].id = i + 1;
        coders_info[i].num_compiles = 0;
        coders_info[i].last_compile.tv_sec = 0;
        coders_info[i].last_compile.tv_nsec = 0;
        coders_info[i].data = data;
        coders_info[i].left_dongle = &dongles[i];
        coders_info[i].right_dongle = &dongles[(i + 1) % data->coder_num];
    }
    return (coders_info);
}

struct monitor_state *init_monitor_state(struct shared_data *data)
{
    struct monitor_state *monitor_info;

    monitor_info = malloc(sizeof(*monitor_info));
    if (!monitor_info)
        return (printf("Error allocation monitor data\n"), NULL);
    monitor_info->id = 0;
    monitor_info->total_required = data->coder_num * data->comp_required;
    monitor_info->data = data;
    return (monitor_info);
}

int init_threads(pthread_t *threads, struct coders_state *coders_info,
                 void *(*coder_func)(void *), struct monitor_state *monitor_info,
                 void *(*monitor_func)(void *))
{
    int i;

    i = 0;
    if (pthread_create(&threads[0], NULL, monitor_func, (void *)monitor_info))
        return (printf("Error creating monitor thread\n"), 0);
    while (++i <= coders_info->data->coder_num)
        if (pthread_create(&threads[i], NULL, coder_func, (void *)&coders_info[i - 1]))
            return (printf("Error creating coder thread\n"), 0);
    return (1);
}
