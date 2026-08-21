#include "codexion.h"

void free_dongles(struct dongle *dongles, int coder_num)
{
    int i;

    i = -1;
    while (++i < coder_num)
    {
        pthread_mutex_destroy(&dongles[i].dongle_mutex);
        pthread_cond_destroy(&dongles[i].dongle_cond);
        free(dongles[i].requests);
    }
    free(dongles);
}

void free_all(struct coders_state *coder_info, struct monitor_state *monitor_info,
              pthread_t *threads)
{
    free_dongles(coder_info->data->dongles, coder_info->data->coder_num);
    pthread_mutex_destroy(&coder_info->data->output_mutex);
    pthread_mutex_destroy(&coder_info->data->queue_mutex);
    pthread_cond_destroy(&coder_info->data->queue_cond);
    free(coder_info->data->deadline);
    free(coder_info->data);
    free(coder_info);
    free(monitor_info);
    free(threads);
}
