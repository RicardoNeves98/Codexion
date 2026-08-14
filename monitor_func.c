#include "codexion.h"

void *monitor_func(void *monitor_state)
{
    int value;
    struct timespec now;
    struct monitor_thread *monitor_info;

    monitor_info = (struct monitor_thread *)monitor_info;
    pthread_mutex_lock(&monitor_info->data->state);
    while (monitor_info->total_comp < monitor_info->total_required)
    {
        value = pthread_cond_timedwait(&monitor_info->data->cond,
                                       &monitor_info->data->state,
                                       &monitor_info->data->deadline->time);
        if (value == ETIMEDOUT)
        {
            clock_gettime(CLOCK_MONOTONIC, &now);
            printf("%d %d burned out", get_time_diff(monitor_info->data->start_time, now),
                   monitor_info->data->deadline->id);
            break
        }
        else
            monitor_info->total_comp += 1;
    }
    return (NULL);
}
