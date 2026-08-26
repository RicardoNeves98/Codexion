#include "codexion.h"

int check_aval(struct dongle *curr_dongle, struct coders_state *coder_info)
{
    struct timespec now;
    
    pthread_mutex_lock(&coder_info->data->output_mutex);
    if (!coder_info->data->active)
    {
        pthread_mutex_unlock(&coder_info->data->output_mutex);
        return (-1);
    }
    pthread_mutex_unlock(&coder_info->data->output_mutex);
    if (curr_dongle->requests->id != coder_info->id)
        return (0);
    if (!curr_dongle->is_free)
        return (0);
    clock_gettime(CLOCK_REALTIME, &now);
    if (get_time_diff(curr_dongle->next_aval, now) > 0)
        return (0);
    return (1);
}

int wait_aval(struct dongle *curr_dongle, struct coders_state *coder_info,
              struct timespec time_limit)
{
    int value;
    int aval_state;
    struct timespec min_time;

    min_time = get_min_time(curr_dongle->next_aval, time_limit);
    aval_state = check_aval(curr_dongle, coder_info);
    while (!aval_state)
    {
        value = pthread_cond_timedwait(&curr_dongle->dongle_cond,
                                       &curr_dongle->dongle_mutex, &time_limit);
        if (value == ETIMEDOUT)
        {
            if (get_time_diff(min_time, time_limit) == 0)
                return (0);
            min_time = time_limit;
        }
        aval_state = check_aval(curr_dongle, coder_info);
    }
    return (aval_state);
}

int get_dongle(struct coders_state *coder_info, struct dongle *curr_dongle,
               struct timespec time_limit)
{
    int dongle_aval;
    struct timespec now;

    pthread_mutex_lock(&curr_dongle->dongle_mutex);
    dongle_aval = wait_aval(curr_dongle, coder_info, time_limit);
    if (dongle_aval == 0 || dongle_aval == -1)
    {
        pthread_mutex_unlock(&curr_dongle->dongle_mutex);
        return (dongle_aval);
    }
    curr_dongle->is_free = 0;
    update_requests(curr_dongle->requests);
    pthread_cond_signal(&curr_dongle->dongle_cond);
    pthread_mutex_unlock(&curr_dongle->dongle_mutex);
    pthread_mutex_lock(&coder_info->data->output_mutex);
    if (coder_info->data->active)
    {
        clock_gettime(CLOCK_MONOTONIC, &now);
        printf("%ld %d has taken dongle%d\n",
               get_time_diff(now, coder_info->data->start_time), coder_info->id,
               curr_dongle->id);
    }
    pthread_mutex_unlock(&coder_info->data->output_mutex);
    return (dongle_aval);
}
