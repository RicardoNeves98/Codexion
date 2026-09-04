#include "codexion.h"

int init_threads(pthread_t *threads, struct coders_state *coders_info,
                 void *(*coder_func)(void *), struct monitor_state *monitor_info,
                 void *(*monitor_func)(void *))
{
    int i;

    i = 0;
    if (pthread_create(&threads[0], NULL, monitor_func, (void *)monitor_info))
        return (printf("Error creating monitor thread\n"), 0);
    while (++i <= coders_info->data->coder_num)
        if (pthread_create(&threads[i], NULL, coder_func, (void *)&coders_info[i - 1]))
            return (printf("Error creating coder thread\n"), 0);
    return (1);
}
