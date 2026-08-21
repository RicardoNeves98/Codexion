#include "codexion.h"

int check_aval(struct dongle *curr_dongle)
{
    struct timespec now;

    if (!curr_dongle->is_free)
        return (0);
    clock_gettime(CLOCK_REALTIME, &now);
    if (get_time_diff(curr_dongle->next_aval, now) > 0)
        return (0);
    return (1);
}

int wait_aval(struct dongle *curr_dongle, int coder_id, struct timespec time_limit)
{
    int value;
    struct timespec min_time;

    min_time = get_min_time(curr_dongle->next_aval, time_limit);    
    while (curr_dongle->requests->id != coder_id && !check_aval(curr_dongle))
    {
        value = pthread_cond_timedwait(&curr_dongle->dongle_cond,
                                       &curr_dongle->dongle_mutex, &time_limit);
        if (value == ETIMEDOUT)
        {
            if (get_time_diff(min_time, time_limit) == 0)
                return (0);
            min_time = time_limit;
        }
    }
    return (1);
}

int get_dongle(struct dongle *curr_dongle, pthread_mutex_t output_mutex, int coder_id,
               struct timespec start, struct timespec time_limit)
{
    struct timespec now;

    pthread_mutex_lock(&curr_dongle->dongle_mutex);
    if (!wait_aval(curr_dongle, coder_id, time_limit))
    {
        pthread_mutex_unlock(&curr_dongle->dongle_mutex);
        return (0);
    }
    curr_dongle->is_free = 0;
    update_requests(curr_dongle->requests);
    pthread_cond_signal(&curr_dongle->dongle_cond);
    pthread_mutex_unlock(&curr_dongle->dongle_mutex);
    pthread_mutex_lock(&output_mutex);
    clock_gettime(CLOCK_MONOTONIC, &now);
    printf("%ld %d has taken a dongle\n", get_time_diff(now, start), coder_id);
    pthread_mutex_unlock(&output_mutex);
    return (1);
}
