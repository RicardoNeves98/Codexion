#include "codexion.h"  

void update_deadline_queue(struct coders_state *coder_info, int compile)
{
    int finished;

    pthread_mutex_lock(&coder_info->data->queue_mutex);
    if (compile == 1)
    {
        finished = 0;
        if (coder_info->num_compiles == coder_info->data->comp_required)
            finished = 1;
        comp_deadline_update(coder_info->data->deadline, coder_info->data->coder_num,
                             coder_info->id, coder_info->data->comp_burnout, finished);
    }
    else
        first_deadline_update(coder_info->data->deadline, coder_info->id,
                              coder_info->data->start_burnout);
    pthread_cond_signal(&coder_info->data->queue_cond);
    pthread_mutex_unlock(&coder_info->data->queue_mutex);
}

void update_dongle_state(struct dongle *curr_dongle)
{
    pthread_mutex_lock(&curr_dongle->dongle_mutex);
    curr_dongle->is_free = 1;
    curr_dongle->next_aval = add_curr_time(curr_dongle->cooldown);
    pthread_cond_signal(&curr_dongle->dongle_cond);
    pthread_mutex_unlock(&curr_dongle->dongle_mutex);
}

int check_coders(struct coders_state *coder_info)
{
    int active;

    active = 1; 
    pthread_mutex_lock(&coder_info->data->queue_mutex);
    if (coder_info->data->coders_active == 0)
        active = 0;
    pthread_mutex_unlock(&coder_info->data->queue_mutex);
    return (active);
}

void write_output(char *type, struct coders_state *coder_info)
{
    struct timespec now;

    if (check_coders(coder_info))
    {
        pthread_mutex_lock(&coder_info->data->output_mutex);
        clock_gettime(CLOCK_MONOTONIC, &now);
        printf("%ld %d is %s\n", get_time_diff(now, coder_info->data->start_time),
               coder_info->id, type);
        pthread_mutex_unlock(&coder_info->data->output_mutex);
    }
}

void go_work(struct coders_state *coder_info)
{
    coder_info->num_compiles += 1;
    update_deadline_queue(coder_info, 1);
    write_output("compiling", coder_info);
    usleep(coder_info->data->time_to_compile * 1000);
    pthread_mutex_lock(&coder_info->data->queue_mutex);
    clock_gettime(CLOCK_REALTIME, &coder_info->last_compile[coder_info->id - 1]);
    pthread_mutex_unlock(&coder_info->data->queue_mutex);
    update_dongle_state(coder_info->left_dongle);
    update_dongle_state(coder_info->right_dongle);
    write_output("debuggin", coder_info);
    usleep(coder_info->data->time_to_debug * 1000);
    write_output("refactoring", coder_info);
    usleep(coder_info->data->time_to_refactor * 1000);
}
