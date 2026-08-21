#include "codexion.h"

void *monitor_func(void *info)
{
    int value;
    struct timespec now;
    struct monitor_state *monitor_info;

    monitor_info = (struct monitor_state *)info;
    usleep(timespec_to_ms(monitor_info->data->time_to_burnout) * 500);
    pthread_mutex_lock(&monitor_info->data->queue_mutex);
    while (monitor_info->total_comp < monitor_info->total_required)
    {
        value = pthread_cond_timedwait(&monitor_info->data->queue_cond,
                                       &monitor_info->data->queue_mutex,
                                       &monitor_info->data->deadline->time);
        if (value == ETIMEDOUT)
        {
            clock_gettime(CLOCK_MONOTONIC, &now);
            printf("%ld %d burned out\n",
                   get_time_diff(monitor_info->data->start_time, now),
                   monitor_info->data->deadline->id);
            break;
        }
        else
            monitor_info->total_comp += 1;
    }
    return (NULL);
}
