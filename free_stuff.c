#include "codexion.h"

void free_dongles(struct dongle *dongles, int index)
{
    int i;

    i = -1;
    while (++i < index)
    {
        pthread_mutex_destroy(&dongles[i].dongle_mutex);
        pthread_cond_destroy(&dongles[i].dongle_cond);
        free(dongles[i].line);
    }
    free(dongles);
}

void free_shared_data(struct shared_data *data)
{
    pthread_mutex_destroy(&data->output_mutex);
    pthread_mutex_destroy(&data->queue_mutex);
    pthread_cond_destroy(&data->queue_cond);
    free(data->deadline);
    free(data);
}

void free_all(struct coders_state *coder_info, struct monitor_state *monitor_info,
              pthread_t *threads)
{
    free_dongles(coder_info->data->dongles, coder_info->data->coder_num);
    free_shared_data(coder_info->data);
    free(coder_info);
    free(monitor_info);
    free(threads);
}
