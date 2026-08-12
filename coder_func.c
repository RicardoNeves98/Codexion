#include "codexion.h"

void go_work(struct coder_thread *coder_info, struct dongle *dongle1,
             struct dongle *dongle2)
{
    pthread_mutex_lock(&coder_info->data->output);
    printf("%d is compiling\n", coder_info->id);
    pthread_mutex_unlock(&coder_info->data->output);
    usleep(coder_info->data->time_to_compile * 1000);
    pthread_mutex_lock(&dongle1->state);
    dongle1->is_free = 1;
    update_next_aval(dongle1);
    pthread_cond_signal(&dongle1->available);
    pthread_mutex_unlock(&dongle1->state);
    pthread_mutex_lock(&dongle2->state);
    dongle2->is_free = 1;
    update_next_aval(dongle2);
    pthread_cond_signal(&dongle2->available);
    pthread_mutex_unlock(&dongle2->state);
    coder_info->data->number_of_compiles[coder_info->id] += 1;
    pthread_mutex_lock(&coder_info->data->output);
    printf("%d is debugging\n", coder_info->id);
    pthread_mutex_unlock(&coder_info->data->output);
    usleep(coder_info->data->time_to_debug * 1000);
    pthread_mutex_lock(&coder_info->data->output);
    printf("%d is refactoring\n", coder_info->id);
    pthread_mutex_unlock(&coder_info->data->output);
    usleep(coder_info->data->time_to_refactor * 1000);
}

int check_aval(struct dongle *curr_dongle)
{
    struct timespec now;

    if (!curr_dongle->is_free)
        return (0);
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (cmp_time(now, curr_dongle->next_aval));
}

void make_request(struct dongle *dongle1, struct dongle *dongle2, int scheduler,
                  int id, struct timespec time)
{
    pthread_mutex_lock(&dongle1->state);
    if (!scheduler)
        fifo(&dongle1->queue, id);
    else
        edf(&dongle1->queue, id, time);
    pthread_mutex_unlock(&dongle1->state);
    pthread_mutex_lock(&dongle2->state);
    if (!scheduler)
        fifo(&dongle2->queue, id);
    else
        edf(&dongle2->queue, id, time);
    pthread_mutex_unlock(&dongle2->state);
}

int get_dongle(struct dongle *curr_dongle, int coder_id, int max_wait)
{
    struct timespec now;
    struct timespec time_limit;

    pthread_mutex_lock(&curr_dongle->state);
    clock_gettime(CLOCK_MONOTONIC, &now);
    time_limit = add_time(now, max_wait);
    while (curr_dongle->queue->id != coder_id)
        pthread_cond_timedwait(&curr_dongle->next,
                               &curr_dongle->state, &time_limit);
        clock_gettime(CLOCK_MONOTONIC, &now);
        if (cmp_time(now, time_limit))
            return (0);
    clock_gettime(CLOCK_MONOTONIC, &now);
    time_limit = add_time(now, max_wait);
    while (!check_aval(curr_dongle))
        pthread_cond_timedwait(&curr_dongle->available,
                               &curr_dongle->state, &time_limit);
        if (cmp_time(now, time_limit))
            return (0);
    curr_dongle->is_free = 0;
    printf("%d has taken a dongle\n", coder_id);
    update_queue(curr_dongle);
    pthread_mutex_unlock(&curr_dongle->state);
    return (1);
}

int check_next(struct dongle *dongle1, struct dongle *dongle2, int coder_id)
{
    pthread_mutex_lock(&dongle1->state);
    if (dongle1->queue->id == coder_id)
        return (1);
    pthread_mutex_unlock(&dongle1->state);
    pthread_mutex_lock(&dongle2->state);
    if (dongle2->queue->id == coder_id)
        return (1);
    pthread_mutex_unlock(&dongle2->state);
    return (0);
}

void *coder_func(void *coder_state)
{
    int dongle_num;
    struct dongle *curr_dongle;
    struct dongle *next_dongle;
    struct coder_thread *coder_info;

    dongle_num = 0;
    coder_info = (struct coder_thread *)coder_state;
    curr_dongle = coder_info->left_dongle;
    next_dongle = coder_info->right_dongle;
    while (1)
    {
        make_request(curr_dongle, next_dongle, coder_info->data->scheduler,
                     coder_info->id, coder_info->last_compile);
        while (dongle_num < 2)
        {
            if (get_dongle(curr_dongle, coder_info->id, coder_info->data->max_wait))
            {
                dongle_num += 1;
                switch_dongles(&curr_dongle, &next_dongle);
            }
            else if (dongle_num == 1 || check_next(curr_dongle, next_dongle))
            {
                remove_requests(curr_dongle, next_dongle, coder_info->id)
                dongle_num == 0;
                break;
            }
        }
        go_work(coder_info, curr_dongle, next_dongle);
    }
    return (NULL);
}
