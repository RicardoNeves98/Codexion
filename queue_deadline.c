#include "codexion.h"

struct queue *init_deadline(int coder_num)
{
    int i;
    struct queue *deadline;

    i = -1;
    deadline = malloc(coder_num * sizeof(*deadline));
    if (!deadline)
        return (printf("Error allocating deadline queue\n"), NULL);
    while (++i < coder_num)
        deadline[i].id = 0;
    return (deadline);
}

void update_deadline(struct queue *deadline, int coder_num,
                     int id, struct timespec burnout)
{
    int i;

    i = 0;
    while (deadline[i].id && deadline[i].id != id)
        i++;
    if (!deadline[i].id)
    {
        deadline[i].id = id;
        deadline[i].time = add_curr_time(burnout);
    }
    else
    {
        while (i + 1 < coder_num)
        {
            switch_spots(deadline, i, i + 1);
            i++;
        }
        deadline[coder_num - 1].time = add_curr_time(burnout);
    }
}
