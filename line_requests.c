#include "codexion.h"

void place_fifo_request(int *line, struct coders_state *coder_info)
{
    if (!line[0])
        line[0] = coder_info->id;
    else
        line[1] = coder_info->id;
}

void place_edf_request(int *line, struct coders_state *coder_info)
{
    int hold;    
    struct timespec coder0_last_comp;
    struct timespec coder1_last_comp;

    if (!line[0])
        line[0] = coder_info->id;
    else
    {
        line[1] = coder_info->id;
        pthread_mutex_lock(&coder_info->data->queue_mutex);
        coder0_last_comp = coder_info->last_compile[line[0] - 1];
        coder1_last_comp = coder_info->last_compile[coder_info->id - 1];
        if (get_time_diff(coder1_last_comp, coder0_last_comp) < 0)
        {
            hold = line[0];
            line[0] = line[1];
            line[1] = hold;
        }
        pthread_mutex_unlock(&coder_info->data->queue_mutex);
    }
}

void update_line(int *line)
{
    line[0] = line[1];
    line[1] = 0;
}

int remove_requests(int *line, int coder_id)
{
    if (line[0] == coder_id)
    {
        update_line(line);
        return (1);
    }
    else if (line[1] == coder_id)
        line[1] = 0;
    return (0);
}
