#include "codexion.h"

void final_output(struct monitor_state *monitor_info, int burnout)
{
    struct timespec now;

    pthread_mutex_unlock(&monitor_info->data->queue_mutex);
    pthread_mutex_lock(&monitor_info->data->output_mutex);
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (burnout)
        printf("%ld %d burned out\n",
               get_time_diff(now, monitor_info->data->start_time),
               monitor_info->data->deadline->id);
    else
        printf("%ld All coders compiled %d times!\n",
               get_time_diff(now, monitor_info->data->start_time),
               monitor_info->data->comp_required);
    monitor_info->data->active = 0;
    pthread_mutex_unlock(&monitor_info->data->output_mutex);
}

void *monitor_func(void *info)
{
    int value;
    struct monitor_state *monitor_info;

    monitor_info = (struct monitor_state *)info;
    pthread_mutex_lock(&monitor_info->data->queue_mutex);
    while (!monitor_info->data->deadline[monitor_info->data->coder_num - 1].id)
        pthread_cond_wait(&monitor_info->data->queue_cond,
                          &monitor_info->data->queue_mutex);
    while (monitor_info->data->total_comp < monitor_info->total_required)
    {
        value = pthread_cond_timedwait(&monitor_info->data->queue_cond,
                                       &monitor_info->data->queue_mutex,
                                       &monitor_info->data->deadline->time);
        if (value == ETIMEDOUT)
            return (final_output(monitor_info, 1), NULL);
    }
    return (final_output(monitor_info, 0), NULL);
}
