#include "codexion.h"

void free_dongles(struct dongle *dongles, int coder_num)
{
    int i;

    i = -1;
    while (++i < coder_num)
    {
        pthread_mutex_destroy(&dongles[i].next);
        pthread_mutex_destroy(&dongles[i].state);
        pthread_cond_destroy(&dongles[i].available);
        free(dongles[i].requests);
    }
    free(dongles);
}

void free_all(struct coders_state *coders_info, struct monitor_state *monitor_info,
              pthread_t *threads)
{
    free_dongles(coders_info->data->dongles, coder_info->data->coder_num);
    pthread_mutex_destroy(&coders_info->data->output);
    pthread_mutex_destroy(&coders_info->data->state);
    pthread_cond_destroy(&coders_info->data->cond);
    free(coders_info->data->deadline);
    free(coder_info->data);
    free(coders_info);
    free(monitor_info);
    free(threads);
}
