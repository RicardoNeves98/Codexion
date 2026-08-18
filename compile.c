#include "codexion.h"

void update_deadline_queue(struct coder_thread *coder_info)
{
    pthread_mutex_lock(&coder_info->data->state);
    update_deadline(coder_info->data->deadline, coder_info->coder_num,
                    coder_info->id, &coder_info->data->time_to_burnout);
    pthread_mutex_unlock(&coder_info->data->state);
}

void update_dongle_state(struct dongle *curr_dongle)
{
    pthread_mutex_lock(&curr_dongle->state);
    curr_dongle->is_free = 1;
    curr_dongle->next_aval = add_curr_time(curr_dongle->cooldown);
    pthread_cond_signal(&curr_dongle->available);
    pthread_mutex_unlock(&curr_dongle->state);
}

void write_output(char *type, struct coder_thread *coder_info)
{
    struct timespec now;

    pthread_mutex_lock(&coder_info->data->output);
    clock_gettime(CLOCK_MONOTONIC, &now);
    printf("%d %d is %s\n", get_time_diff(coder_info->data->start_time, now),
           coder_info->id, type);
    pthread_mutex_unlock(&coder_info->data->output);
}

void go_work(struct coder_thread *coder_info)
{
    update_deadline_queue(coder_info);
    write_output("compiling", coder_info);
    usleep(coder_info->data->time_to_compile * 1000);
    clock_gettime(CLOCK_MONOTONIC, &coder_info->last_compile);
    update_dongle_state(coder_info->left_dongle);
    update_dongle_state(coder_info->right_dongle);
    coder_info->num_compiles += 1;
    write_output("debuggin", coder_info);
    usleep(coder_info->data->time_to_debug * 1000);
    write_output("refactoring", coder_info);
    usleep(coder_info->data->time_to_refactor * 1000);
}
