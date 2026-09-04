#include "codexion.h"

void make_request(struct dongle *left_dongle, struct dongle *right_dongle,
                  struct coders_state *coder_info)
{
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    coder_info->data->place_request(left_dongle->line, coder_info);
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    coder_info->data->place_request(right_dongle->line, coder_info);
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
}
    
void delete_requests(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, int has_dongle)
{
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    if (has_dongle)
        left_dongle->is_free = 1;
    if (remove_requests(left_dongle->line, coder_id))
        pthread_cond_signal(&left_dongle->dongle_cond);
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    if (remove_requests(right_dongle->line, coder_id))
        pthread_cond_signal(&right_dongle->dongle_cond);
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
    usleep(5);
}

int check_queues(struct dongle *left_dongle, struct dongle *right_dongle,
                 int coder_id)
{
    int first_in_line;

    first_in_line = 0;
    pthread_mutex_lock(&left_dongle->dongle_mutex);
    if (left_dongle->line[0] == coder_id)
        first_in_line = 1;
    pthread_mutex_unlock(&left_dongle->dongle_mutex);
    pthread_mutex_lock(&right_dongle->dongle_mutex);
    if (right_dongle->line[0] == coder_id)
        first_in_line = 1;
    pthread_mutex_unlock(&right_dongle->dongle_mutex);
    return (first_in_line);
}

void get_both_dongles(struct dongle *left_dongle, struct dongle *right_dongle,
                      struct coders_state *coder_info)
{
    struct timespec time_limit;

    time_limit = add_curr_time(coder_info->data->max_wait);
    if (get_dongle(coder_info, left_dongle, time_limit))
    {
        if (get_dongle(coder_info, right_dongle, time_limit))
            go_work(coder_info);
        else
            delete_requests(left_dongle, right_dongle, coder_info->id, 1);
    }
    else if (check_queues(left_dongle, right_dongle, coder_info->id))
        delete_requests(left_dongle, right_dongle, coder_info->id, 0);
}

int check_active(struct coders_state *coder_info)
{
    int state;

    state = 1;
    pthread_mutex_lock(&coder_info->data->queue_mutex);
    if (coder_info->data->coders_active == 0)
        state = 0;
    else if (coder_info->num_compiles == coder_info->data->comp_required)
    {
        coder_info->data->coders_active -= 1;
        pthread_cond_signal(&coder_info->data->queue_cond);
        state = 0;
    }
    pthread_mutex_unlock(&coder_info->data->queue_mutex);
    return (state);
}

void *coder_func(void *info)
{
    struct coders_state *coder_info;

    coder_info = (struct coders_state *)info;
    update_deadline_queue(coder_info, 0);
    while (check_active(coder_info))
    {
        make_request(coder_info->left_dongle, coder_info->right_dongle, coder_info);
        get_both_dongles(coder_info->left_dongle, coder_info->right_dongle,
                         coder_info);
    }
    return (NULL);
}
