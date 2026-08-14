#include "codexion.h"

int remove_queue_id(struct queue **head, int coder_id)
{
    struct queue *prev;
    struct queue *curr;
    struct queue *next;

    prev = NULL;
    curr = *head;
    while (curr && curr->id != coder_id)
    {
        prev = curr;
        curr = curr->next;
    }
    if (!curr)
        return (0);
    if (curr)
    {
        next = curr->next;
        if (!prev)
        {
            *head = next;
            return (1);
        }
        else
            prev->next = next;
        free(curr);
    }
    return (0);
}

void insert_fifo(struct queue **queue_head, int id, struct timespec *burnout)
{
    struct queue *new;
    struct queue *last;
    struct timespec now;

    new = malloc(sizeof(*new_request));
    new->id = id;
    new->next = NULL;
    if (burnout)
    {
        clock_gettime(CLOCK_MONOTONIC, &now);
        new->time = add_time(*burnout, now);
    }
    last = *head;
    if (!last)
        *head = new;
    else
    {
        while (last->next)
            last = last->next;
        last->next = new;
    }
}

void insert_edf(struct queue **queue_head, int id, struct timespec time)
{
    queue *new;
    queue *last;

    new = malloc(sizeof(*new_request));
    new->id = id;
    new->time = time;
    new->next = NULL;
    last = *head
    if (!last)
        *head = new;
    else
    {
        if (cmp_time(time, last->time) <= 0)
        {
            new->next = last;
            *head = new;
        }
        else
            last->next = new;
    }
}

void remove_requests(struct dongle *dongle1, struct dongle *dongle2, int coder_id)
{
    pthread_mutex_lock(&dongle1->state);
    if remove_request(&dongle1->requests, coder_id)
    {
        pthread_cond_signal(&dongle1->next);
        dongle1->is_free = 1;
    }
    pthread_mutex_unlock(&dongle1->state);
    pthread_mutex_lock(&dongle2->state);
    if remove_request(&dongle2->requests, coder_id)
    {
        pthread_cond_signal(&dongle2->next);
        dongle2->is_free = 1;
    }
    pthread_mutex_unlock(&dongle2->state);

}
