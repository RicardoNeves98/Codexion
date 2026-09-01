#include "codexion.h"

struct queue *init_deadline(int coder_num, struct timespec start_burnout)
{
    int i;
    struct queue *deadline;

    i = -1;
    deadline = malloc(coder_num * sizeof(*deadline));
    if (!deadline)
        return (printf("Error allocating deadline queue\n"), NULL);
    while (++i < coder_num)
    {
        deadline[i].id = 0;
        deadline[i].time = add_curr_time(start_burnout);
    }
    return (deadline);
}

void update_deadline(struct queue *deadline, int coder_num,
                     int id, struct timespec burnout, int finished)
{
    int i;
    int j;

    i = 0;
    while (deadline[i].id != 0 && deadline[i].id != id)
        i++;
    deadline[i].time = add_curr_time(burnout);
    if (!deadline[i].id)
        deadline[i].id = id;
    else
    {
        if (finished)
            deadline[i].id = -1;
        j = 0;
        while (j < coder_num && deadline[j].id != 0 && deadline[j].id != -1)
            j++;
        while (i + 1 < j)
        {
            switch_spots(deadline, i, i + 1);
            i++;
        }
    }
}

void print_deadline(struct queue *deadline, int coder_num)
{
    int i;
    struct timespec now;

    i = -1;
    while (++i < coder_num)
        printf("Spot %d -> Coder %d Time %ld sec %ld nsec\n",
               i, deadline[i].id, deadline[i].time.tv_sec, deadline[i].time.tv_nsec);
    clock_gettime(CLOCK_REALTIME, &now);
    printf("Current time -> %ld sec %ld nsec\n", now.tv_sec, now.tv_nsec);
}

