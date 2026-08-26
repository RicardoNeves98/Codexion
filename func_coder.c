#include "codexion.h"

void make_request(struct dongle *left_dongle, struct dongle *right_dongle,
                  int id, int scheduler, struct timespec last_compile)
{
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    if (!scheduler)
        insert_fifo(left_dongle->requests, id);
    else
        insert_edf(left_dongle->requests, id, last_compile);
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    if (!scheduler)
        insert_fifo(right_dongle->requests, id);
    else
        insert_edf(right_dongle->requests, id, last_compile);
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
}

void delete_requests(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, int has_dongle)
{
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    if (has_dongle)
    {
        left_dongle->is_free = 1;
        printf("%d Released a dongle\n", coder_id);
    }
    if (remove_requests(left_dongle->requests, coder_id))
        pthread_cond_signal(&left_dongle->dongle_cond);
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    if (remove_requests(right_dongle->requests, coder_id))
        pthread_cond_signal(&right_dongle->dongle_cond);
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
    usleep(500);
}

int check_queues(struct dongle *left_dongle, struct dongle *right_dongle,
                 int coder_id)
{
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    if (left_dongle->requests->id == coder_id)
    {
        pthread_mutex_unlock(&left_dongle->dongle_mutex);
        return (1);
    }
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    if (right_dongle->requests->id == coder_id)
    {
        pthread_mutex_unlock(&right_dongle->dongle_mutex);
        return (1);
    }
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
    return (0);
}

void get_both_dongles(struct coders_state *coder_info)
{
    int got_dongle;
    struct timespec time_limit;
    struct dongle *left_dongle;
    struct dongle *right_dongle;

    left_dongle = coder_info->left_dongle;
    right_dongle = coder_info->right_dongle;
    time_limit = add_curr_time(coder_info->data->max_wait);
    got_dongle = get_dongle(coder_info, left_dongle, time_limit);
    if (got_dongle == -1)
        return ;
    else if (got_dongle == 1)
    {
        got_dongle = get_dongle(coder_info, right_dongle, time_limit);
        if (got_dongle == -1)
            return ;
        else if (got_dongle == 1)
            go_work(coder_info);
        else
            delete_requests(left_dongle, right_dongle, coder_info->id, 1);
    }
    else if (check_queues(left_dongle, right_dongle, coder_info->id))
        delete_requests(left_dongle, right_dongle, coder_info->id, 0);
}

int check_active(struct coders_state *coder_info)
{
    pthread_mutex_lock(&coder_info->data->output_mutex);
    if (!coder_info->data->active)
        return (0);
    pthread_mutex_unlock(&coder_info->data->output_mutex);
    if (coder_info->num_compiles == coder_info->data->comp_required)
        return (0);
    return (1);
}

void init_deadline_queue(struct coders_state *coder_info)
{
    pthread_mutex_lock(&coder_info->data->queue_mutex);
    update_deadline(coder_info->data->deadline, coder_info->data->coder_num,
                    coder_info->id, coder_info->data->time_to_burnout);
    if (coder_info->data->deadline[coder_info->data->coder_num - 1].id)
        pthread_cond_signal(&coder_info->data->queue_cond);
    pthread_mutex_unlock(&coder_info->data->queue_mutex);
}

void *coder_func(void *info)
{
    struct timespec now;
    struct coders_state *coder_info;

    coder_info = (struct coders_state *)info;
    init_deadline_queue(coder_info);
    while (check_active(coder_info))
    {
        make_request(coder_info->left_dongle, coder_info->right_dongle, coder_info->id,
                     coder_info->data->scheduler, coder_info->last_compile);
        clock_gettime(CLOCK_MONOTONIC, &now);
        printf("%ld coder %d made request\n",
               get_time_diff(now, coder_info->data->start_time), coder_info->id);
        get_both_dongles(coder_info);
    }
    return (printf("Coder %d End\n", coder_info->id), NULL);
}
