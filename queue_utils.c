#include "codexion.h"

void update_queue(struct dongle *curr_dongle)
{
    queue *first_request;

    first_request = curr_dongle->next_to_use->next;
    free(curr_dongle->next_to_use);
    curr_dongle->next_to_use = first_request;
    pthread_cond_signal(&dongle->next);
}

void remove_request(struct dongle *curr_dongle, int coder_id)
{
    queue *prev_request;
    queue *curr_request;
    queue *next_request;

    pthread_mutex_lock(&curr_dongle->state);
    prev_request = NULL;
    curr_request = curr_dongle->next_to_use;
    while (curr_request && curr_request->id != coder_id && curr_request->next)
    {
        prev_request = curr_request;
        curr_request = curr_request->next;
    }
    if (curr_request)
    {
        next_request = curr_request->next;
        if (!prev_request)
        {
            curr_dongle->next_to_use = next_request;
            pthread_cond_signal(curr_dongle->next);
        }
        else
            prev_request->next = next_request;
        free(curr_request);
    }
    curr_dongle->is_free = 1;
    pthread_mutex_unlock(&curr_dongle->state);
}

void remove_requests(struct dongle *dongle1, struct dongle *dongle2, int coder_id)
{
    remove_request(dongle1, coder_id);
    remove_request(dongle2, coder_id);
}
