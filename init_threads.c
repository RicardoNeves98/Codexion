#include "codexion.h"

shared_config *init_config(int *parsed_args, int coder_num)
{
    int i;
    int *compile_arr;
    int *dongle_arr;
    shared_config *config;

    i = -1;
    compile_arr = malloc(coder_num * sizeof(*compile_arr));
    dongle_arr = malloc(coder_num * sizeof(*dongle_arr));
    config = malloc(sizeof(*config));
    if (!compile_arr || !dongle_arr || !config)
        return (NULL);
    while (++i < coder_num)
    {
        compile_arr[i] = 0;
        dongle_arr[i] = 0;
    }
    config->dongles_available = parsed_args[0];
    config->time_to_burnout = parsed_args[1];
    config->time_to_compile = parsed_args[2];
    config->time_to_debug = parsed_args[3];
    config->time_to_refactor = parsed_args[4];
    config->number_of_compiles_required = parsed_args[5];
    config->dongle_cooldown = parsed_args[6];
    config->number_of_compiles = compile_arr;
    config->number_of_dongles = dongle_arr;
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

int make_single_request(request_queue **head_queue, int coder_id, int number)
{
    int i;
    struct request_queue *queue;
    struct request_queue *new_queue;

    i = -1;
    queue = *head_queue;
    while (queue->next)
        queue = queue->next;
    while (++i < number)
    {
        new_queue = malloc(sizeof(*new_queue));
        if (!new_queue)
            return (0);
        queue->id = coder_id;
        queue->next = new_queue;
        new_queue->id = -1;
        new_queue->next = NULL;
        queue = new_queue;
    }
}

void *coder_func(void *state)
{
    int number;
    struct thread_state *sim_state;

    while (true)
    {
        sim_state = (struct thread_state *)state;
        number = sim_state->config->number_of_dongles[sim_state->id];
        while (number != 2)
        {
            make_request(&(sim_state->queue), sim_state->id, 2 - number);
            pthread_cond_wait(&cod, &m);
            number = number_of_dongles[sim_state->id];
        }
        printf("%d %d is compiling", timestamp, sim_state->id);
        sim_state->config->number_of_dongles[sim_state->id] = 0;
        sim_state->config->dongles_available += 2;
        pthread_cond_signal(&mon);
        sim_state->config->number_of_compiles[sim_state->id] += 1;
        sleep(sim_state->config->time_to_compile);
        printf("%d %d is debugging", timestamp, sim_state->id);
        sleep(sim_state->config->time_to_debug);
        printf("%d %d id refactoring", timestamp, sim_state->id);
        sleep(sim_state->config->time_to_refactor);
    }
}

void *monitor_func(void *state)
{
    struct thread_state *sim_state;

    sim_state = (struct thread_state *)state;
    while (!sim_state->config->dongles_available)
        pthread_cond_wait(&mon, &m);
    
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

