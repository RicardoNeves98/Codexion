#include "codexion.h"

void make_request(struct dongle *dongle1, struct dongle *dongle2,
                  int id, int scheduler, struct timespec time)
{
    pthread_mutex_lock(&dongle1->dongle_mutex);
    if (!scheduler)
        insert_fifo(dongle1->requests, id);
    else
        insert_edf(dongle1->requests, id, time);
    pthread_mutex_unlock(&dongle1->dongle_mutex);
    pthread_mutex_lock(&dongle2->dongle_mutex);
    if (!scheduler)
        insert_fifo(dongle2->requests, id);
    else
        insert_edf(dongle2->requests, id, time);
    pthread_mutex_unlock(&dongle2->dongle_mutex);
}

void delete_requests(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, int dongle_num)
{
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    if (remove_requests(left_dongle->requests, coder_id))
    {
        pthread_cond_signal(&left_dongle->dongle_cond);
        if (dongle_num == 1)
            left_dongle->is_free = 1;
    }
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    if (remove_requests(right_dongle->requests, coder_id))
        pthread_cond_signal(&right_dongle->dongle_cond);
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
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

int get_both_dongles(struct coders_state *coder_info)
{
    int dongle_num;
    struct timespec time_limit;
    struct dongle *curr_dongle;

    dongle_num = 0;
    time_limit = add_curr_time(coder_info->data->max_wait);
    curr_dongle = coder_info->left_dongle;
    while (dongle_num < 2)
    {
        if (get_dongle(curr_dongle, coder_info->data->output_mutex, coder_info->id,
                       coder_info->data->start_time, time_limit))
        {
            dongle_num += 1;
            curr_dongle = coder_info->right_dongle;
        }
        else
            break;
    }
    return (dongle_num);
}

void *coder_func(void *info)
{
    int dongle_num;
    struct coders_state *coder_info;
    struct dongle *left_dongle;
    struct dongle *right_dongle;

    coder_info = (struct coders_state *)info;
    left_dongle = coder_info->left_dongle;
    right_dongle = coder_info->right_dongle;
    update_deadline_queue(coder_info);
    while (coder_info->num_compiles < coder_info->data->comp_required)
    {
        make_request(left_dongle, right_dongle, coder_info->id,
                     coder_info->data->scheduler, coder_info->last_compile);
        dongle_num = get_both_dongles(coder_info);
        if (dongle_num == 2)
            go_work(coder_info);
        else if (dongle_num == 1 ||
                 check_queues(left_dongle, right_dongle, coder_info->id))
            delete_requests(left_dongle, right_dongle, coder_info->id, dongle_num);
    }
    return (NULL);
}
