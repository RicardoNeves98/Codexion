#include "codexion.h"

int main(int argc, char **argv)
{
    int coder_num;
    int *parsed_args;
    pthread_t *threads;

    if (argc != 9)
        return (printf("[ERROR] Needs 8 arguments to run\n"), 1);
    parsed_args = parsing(argv);
    if (!parsed_args)
        return (1);
    coder_num = parsed_args[0];
    threads = malloc((coder_num + 1) * sizeof(*threads));
    if (!threads)
        return (printf("[ERROR] Allocation failed\n"), 1);
    if (!init_threads(threads, state, parsed_args, coder_func, monitor_func))
        return (1);
    while (coder_num--)
        pthread_join(threads[coder_num], NULL);
    pthread_cond_destroy(&state->config->free_dongles);
    pthread_cond_destroy(&state->config->both_dongles);
    pthread_mutex_destroy(&state->config->lock);
}
