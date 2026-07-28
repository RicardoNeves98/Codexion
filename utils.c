#include "codexion.h"

int *get_zeros_arr(int size)
{
    int i;
    int *arr;

    i = 0;
    arr = malloc(size * sizeof(*arr));
    if (!arr)
        return (NULL);
    while (i < size)
        arr[i++] = 0;
    return (arr);
}

int make_request(int coder_id, int dongle_num, coder_thread *coder_info)
{
    struct queue *curr_queue;
    struct queue *new_queue;
    
    if (!dongle_num)
        return (1);
    if (!coder_info->coder_queue)
    {
        new_queue = new_request(coder_id);
        if (!new_queue)
            return (printf("Error\n"), 0);
        coder_info->coder_queue = new_queue;
        dongle_num--;
    }
    curr_queue = new_queue;
    while (dongle_num--)
    {
        new_queue = new_request(coder_id);
        if (!new_queue)
            return (printf("Error\n"), 0);
        curr_queue->next = new_queue;
        curr_queue = new_queue;
    }
    return (1);
}

queue *new_request(int coder_id)
{
    struct queue *new_queue;

    new_queue = malloc(sizeof(*new_queue));
    if (!new_queue)
        return (NULL);
    new_queue->id = coder_id;
    new_queue->next = NULL;
    return (new_queue);
}

int give_dongles(coder_thread *coder_info)
{
    int i;
    int coder_num;
    int assigned_dongles;

    i = -1;
    coder_num = coder_info->data->coder_num;
    assigned_dongles = 0;
    while (++i < coder_num)
    {
        if (coder_info->dongle_list[i].free_dongle)
        {
            give_dongle(i, coder_info);
            assigned_dongles += 1;
        }
    }
    return (assigned_dongles);
}

void give_dongle(int dongle_id, coder_thread *coder_info)
{
    struct queue *new_queue;

    curr_id = coder_info->coder_queue->id;
    oder_info->data->number_of_dongles[curr_id] += 1;
    coder_info->dongles[i].free_dongle = 0;
    coder_info->dongles[i].last_used = ;
    pthread_mutex_lock(&coder_info->dongles[i].dongle_lock);
    pthread_mutex_lock(&coder_info->state->lock);
    printf("%d has taken a dongle\n", curr_id);
    new_queue = coder_info->coder_queue->next;
    free(coder_info->coder_queue);
    coder_info->coder_queue = new_queue;
    pthread_mutex_unlock(&coder_info->state->lock);
}

