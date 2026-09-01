#include "codexion.h"

void signal_dongles(struct monitor_state *monitor_info)
{
    int i;

    i = -1;
    while (++i < monitor_info->data->coder_num)
    {
        pthread_mutex_lock(&monitor_info->data->dongles[i].dongle_mutex);
        pthread_cond_signal(&monitor_info->data->dongles[i].dongle_cond);
        pthread_mutex_unlock(&monitor_info->data->dongles[i].dongle_mutex);
    }
}

void *finish(struct monitor_state *monitor_info)
{
    struct timespec now;

    pthread_mutex_lock(&monitor_info->data->output_mutex);
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (monitor_info->data->coders_active == 0)
    {
        printf("%ld All coders compiled %d times!\n",
               get_time_diff(now, monitor_info->data->start_time),
               monitor_info->data->comp_required);
        pthread_mutex_unlock(&monitor_info->data->output_mutex);
        pthread_mutex_unlock(&monitor_info->data->queue_mutex);
    }
    else
    {
        monitor_info->data->coders_active = 0;
        printf("%ld %d burned out\n",
               get_time_diff(now, monitor_info->data->start_time),
               monitor_info->data->deadline->id);
        pthread_mutex_unlock(&monitor_info->data->output_mutex);
        pthread_mutex_unlock(&monitor_info->data->queue_mutex);
        signal_dongles(monitor_info);
    }
    return (NULL);
}

void *monitor_func(void *info)
{
    int value;
    struct timespec time;
    struct monitor_state *monitor_info;

    monitor_info = (struct monitor_state *)info;
    pthread_mutex_lock(&monitor_info->data->queue_mutex);
    while (monitor_info->data->deadline->id == 0)
        pthread_cond_wait(&monitor_info->data->queue_cond,
                          &monitor_info->data->queue_mutex);
    while (monitor_info->data->coders_active)
    {
        time = monitor_info->data->deadline->time;
        value = pthread_cond_timedwait(&monitor_info->data->queue_cond,
                                       &monitor_info->data->queue_mutex, &time);
        if (value == ETIMEDOUT)
            if (monitor_info->data->deadline->id != 0 &&
                monitor_info->data->deadline->id != -1)
                return (finish(monitor_info));
    }
    return (finish(monitor_info));
}
