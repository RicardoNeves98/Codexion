#include "codexion.h"

int check_aval(struct dongle *curr_dongle)
{
    struct timespec now;

    if (!curr_dongle->is_free)
        return (0);
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (cmp_time(curr_dongle->next_aval, now) > 0)
        return (0)
    return (1);
}

int wait_line(struct dongle *curr_dongle, int coder_id, struct timespec time_limit)
{
    while (curr_dongle->requests->id != coder_id)
    {
        value = pthread_cond_timedwait(&curr_dongle->next, &curr_dongle->state,
                                       &time_limit);
        if (value == ETIMEDOUT)
            return (0);
    }
    return (1);
}

int wait_aval(struct dongle *curr_dongle, int coder_id, struct timespec time_limit)
{
    struct timespec min_time;

    min_time = get_min_time(curr_dongle->next_aval, time_limit);
    while (!check_aval(curr_dongle))
    {
        pthread_cond_timedwait(&curr_dongle->available,
                               &curr_dongle->state, &min_time);
        if (value == ETIMEDOUT)
        {
            if (get_time_diff(min_limit, time_limit) == 0)
                return (0);
            min_time = time_limit;
        }
    }
    return (1);
}

int get_dongle(struct dongle *curr_dongle, int coder_id, struct timespec time_limit)
{
    int value;
    struct timespec min_time;

    pthread_mutex_lock(&curr_dongle->state);
    if (!wait_line(curr_dongle, coder_id, time_limit))
    {
        pthread_mutex_unlock(&curr_dongle->state);
        return (0);
    }
    if (!wait_aval(curr_dongle, coder_id, time_limit))
    {
        pthread_mutex_unlock(&curr_dongle->state);
        return (0);
    }
    curr_dongle->is_free = 0;
    printf("%d has taken a dongle\n", coder_id);
    remove_queue_id(&curr_dongle->requests, coder_id);
    pthread_cond_signal(&curr_dongle->next);
    pthread_mutex_unlock(&curr_dongle->state);
    return (1);
}
