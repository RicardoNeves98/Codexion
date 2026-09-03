#include "codexion.h"

int main(int argc, char **argv)
{
    int i;
    struct shared_data *data;
    struct coders_state *coder_info;
    struct monitor_state *monitor_info;
    pthread_t *threads;

    i = -1;
    data = init_data(parsing(argc, argv));
    if (!data)
        return (1);
    coder_info = init_coders_state(data);
    if (!coder_info)
        return (free(data), 1);
    monitor_info = init_monitor_state(data);
    if (!monitor_info)
        return (free_all(coder_info, NULL, NULL), 1);
    threads = malloc((coder_info->data->coder_num + 1) * sizeof(*threads));
    if (!threads)
    {
        printf("Error allocating threads\n");
        return (free_all(coder_info, monitor_info, NULL), 1);
    }
    if (!init_threads(threads, coder_info, coder_func, monitor_info, monitor_func))
        return (free_all(coder_info, monitor_info, threads), 1);
    while (++i <= coder_info->data->coder_num)
        pthread_join(threads[i], NULL);
    return (free_all(coder_info, monitor_info, threads), 0);
}
