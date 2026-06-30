#include "codexion.h"

int main(int argc, char **argv)
{
    int i;
    int coder_num;
    int *parsed_args;
    pthread_t *threads;
    thread_state *state;

    i = -1;
    if (argc != 9)
        return (printf("[ERROR] Needs 9 arguments\n"), 1);
    parsed_args = parsing(argv);
    if (!parsed_args)
        return (1);
    coder_num = parsed_args[0];
    state = malloc((coder_num + 1) * sizeof(*state));
    threads = malloc((coder_num + 1) * sizeof(*threads));
    if (!state || !threads)
        return (free(parsed_args), printf("[ERROR] Allocation failed\n"), 1);
    if (!init_state(parsed_args, state, coder_num))
        return (printf("[ERROR] Allocation failed\n"), 1);
    if (!init_threads(threads, coder_func, monitor_func, state, coder_num))
        return (1);
}

