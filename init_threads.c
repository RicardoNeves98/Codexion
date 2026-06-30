#include "codexion.h"

shared_config *init_config(int *parsed_args, int coder_num)
{
    int i;
    int *compile_arr;
    shared_config *config;

    i = -1;
    compile_arr = malloc(coder_num * sizeof(*arr));
    if (!compile_arr)
        return (free(parsed_args), NULL);
    if (!config)
        return (free(parsed_args), free(compile_arr), NULL);
    while (++i < coder_num)
        compile_arr[i] = 0;
    config->time_to_burnout = parsed_args[1];
    config->time_to_compile = parsed_args[2];
    config->time_to_debug = parsed_args[3];
    config->time_to_refactor = parsed_args[4];
    config->number_of_compiles_required = parsed_args[5];
    config->dongle_cooldown = parsed_args[6];
    config->scheduler = parsed_args[7];
    config->curr_coder = -1;
    config->number_of_compiles = compile_arr;
    return (config);
}

request_queue *init_queue(void)
{
    request_queue *queue;

    queue = malloc(sizeof(*queue));
    if (!queue)
        return (NULL);
    queue->id = -1;
    queue->next = NULL;
    return (queue);
}

int init_state(pthread_t *threads, thread_state *state, int *parsed_args, int coder_num,
               (void *)(*coder_func)(void *), (void *)(*monitor_func)(void *))
{
    int i;
    shared_config *config;
    request_queue *queue;

    i = -1;
    config = init_config(parsed_args, coder_num);
    queue = init_queue();
    if (!config || !queue)
        return (0);
    while (++i < coder_num)
    {
        state->id = i;
        state->config = config;
        state->queue = queue;
        if pthread_create(threads[i], NULL, coder_func, state)
            return (0);
    }
    state->id = i;
    state->config = config;
    state->queue = queue;
    if pthread_create(threads[i], NULL, monitor_func, state)
        return (0);
    return (1);
}

