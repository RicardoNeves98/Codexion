#include "codexion.h"

int main(int argc, char **argv)
{
    struct shared_data *data;
    struct coders_state *coders_info;
    struct monitor_state *monitor_info;
    pthread_t *threads;

    data = init_data(parsing(argc,argv));
    if (!data)
        return (1);
    coders_info = init_coders_state(data);
    if (!coders_info)
        return (free(data), 1);
    monitor_info = init_monitor_state(data);
    if (!monitor_info)
        return (free_all(coders_info, NULL, NULL), 1);
    threads = malloc((coder_info->data->coder_num + 1) * sizeof(*threads));
    if (!threads)
    {
        printf("Error allocating threads\n");
        return (free_all(coders_info, monitor_info, threads), 1);
    }
    if (!init_threads(coders_info, coder_func, monitor_info, monitor_func))
        return (free_all(coders_info, monitor_info, threads), 1);
    while (coder_num--)
        pthread_join(threads[coder_num], NULL);
    free_all(coders_info, monitor_info, threads);
}
