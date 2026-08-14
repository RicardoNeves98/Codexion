#include "codexion.h"

void make_request(struct dongle *dongle1, struct dongle *dongle2,
                  int id, int scheduler, struct timespec time)
{
    pthread_mutex_lock(&dongle1->state);
    if (!scheduler)
        insert_fifo(&dongle1->requests, id, NULL);
    else
        insert_edf(&dongle1->requests, id, &time);
    pthread_mutex_unlock(&dongle1->state);
    pthread_mutex_lock(&dongle2->state);
    if (!scheduler)
        insert_fifo(&dongle2->requests, id, NULL);
    else
        insert_edf(&dongle2->requests, id, &time);
    pthread_mutex_unlock(&dongle2->state);
}

void start_over(struct dongle *dongle1, struct dongle *dongle2, int coder_id)
{
    int start_clean;

    start_clean = 0;
    pthread_mutex_lock(&dongle1->state);
    if (dongle1->requests->id == coder_id)
        start_clean = 1;
    pthread_mutex_unlock(&dongle1->state);
    pthread_mutex_lock(&dongle2->state);
    if (dongle2->requests->id == coder_id)
        start_clean = 1;
    pthread_mutex_unlock(&dongle2->state);
    if (start_clean)
        remove_requests(dongle1, dongle2, coder_id);
}

int get_both_dongles(struct dongle *left_dongle, struct dongle *right_dongle,
                     int coder_id, struct timespec time_limit)
{
    int dongle_num;
    struct dongle *curr_dongle;

    dongle_num = 0;
    curr_dongle = left_dongle;
    while (dongle_num < 2)
    {
        if (get_dongle(curr_dongle, coder_id, time_limit))
        {
            dongle_num += 1;
            curr_dongle = right_dongle;
        }
        else
            return (0);
    }
    return (1);
}

void *coder_func(void *coder_state)
{
    struct coder_thread *coder_info;

    coder_info = (struct coder_thread *)coder_state;
    while (coder_info->num_compiles < coder_info->data->comp_required)
    {
        make_request(coder_info->left_dongle, coder_info->right_dongle, coder_info->id,
                     coder_info->data->scheudler, coder_info->last_compile);
        time_limit = add_curr_time(coder_info->data->max_wait);
        if (get_both_dongles(coder_info->left_dongle, coder_info->right_dongle,
                             coder_info->id, time_limit))
            go_work(coder_info);
        else
            start_over(coder_info->left_dongle, right_dongle, coder_id);
    }
    return (NULL);
}
