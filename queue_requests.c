#include "codexion.h"

struct queue *init_requests(void)
{
    struct queue *requests;

    requests = malloc(2 * sizeof(*requests));
    if (!requests)
        return (printf("Error allocating requests queue\n"), NULL);
    requests[0].id = 0;
    requests[0].time = NULL;
    requests[1].id = 0;
    requests[1].time = NULL;
    return (requests);
}

void insert_fifo(struct queue *requests, int id)
{
    if (!requests[0].id)
        requests[0].id = id;
    else
        requests[1].id = id;
}

void insert_edf(struct queue *requests, int id, struct timespec *last_compile)
{
    if (!requests[0].id)
    {
        requests[0].id = id;
        requests[0].time = last_compile;
    }
    else if (get_time_diff(requests[0].time, *last_compile) > 0)
    {
        requests[1].id = id;
        requests[1].time = last_compile;
        switch_spots(requests, 0, 1);
    }
    else
    {
        requests[1].id = id;
        requests[1].time = last_compile;
    }
}

void update_requests(struct queue *requests)
{
    requests[0].id = 0
    requests[0].time = NULL;
    switch_spots(requests, 0, 1);
}

int remove_requests(struct queue *requests, int id)
{
    if (requests[0].id == id)
    {
        update_requests(requests);
        return (1);
    }
    else if (requests[1].id == id)
    {
        requests[1].id = 0;
        requests[1].time = NULL;
    }
    return (0);
}
