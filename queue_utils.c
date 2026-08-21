#include "codexion.h"

void switch_spots(struct queue *line, int index1, int index2)
{
    int id_hold;
    struct timespec time_hold;

    id_hold = line[index1].id;
    time_hold = line[index1].time;
    line[index1].id = line[index2].id;
    line[index1].time = line[index2].time;
    line[index2].id = id_hold;
    line[index2].time = time_hold;
}
