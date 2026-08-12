#include "codexion.h"

void fifo(struct queue **queue_head, int id)
{
    struct queue *new_request;
    struct queue *last_request;

    new_request = malloc(sizeof(*new_request)); 
    new_request->id = id;
    new_request->next = NULL;
    if (!(*queue_head))
        **queue_head = new_request;
    else
    {
        last_request = *queue_head;
        last_request->next = new_request;
    }
}

void edf(struct queue **queue_head, int id, struct timespec time)
{
    queue *new_request;
    queue *last_request;

    new_request = malloc(sizeof(*new_request));
    new_request->id = id;
    new_request->time = time;
    new_request->next = NULL;
    if (!(*queue_head))
        *queue_head = new_request;
    else
    {
        last_request = *queue_head;
        if (!cmp_time(curr_request->time, time))
        {
            new_request->next = curr_request;
            *queue_head = new_request;
        }
        else
            curr_request->next = new_request;
    }
}
