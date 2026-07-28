#include "codexion.h"

void *coder_func(void *coder_state)
{
    struct coder_thread *coder_info;

    coder_info = (struct coder_thread *)coder_state;
    while (1)
    {
        pthread_mutex_lock(&coder_info->state->lock);
        make_request(coder_info->thread_id, coder_info->queue);
        pthread_mutex_unlock(&coder_info->state->lock);
        while (coder_info->data->number_of_dongles[coder_info->thread_id] != 2)
            while (!give_dongles(coder_info));
                pthread_cond_broadcast(&coder_info->state->free_dongles);
        printf("%d is compiling\n", coder_info->thread_id);
        usleep(coder_info->state->time_to_compile * 1000);
        pthread_mutex_lock(&coder_info->state->lock);
        coder_info->data->number_of_dongles[coder_info->thread_id] = 0;
        pthread_cond_signal(&coder_info->state->free_dongles);
        pthread_mutex_unlock(&coder_info->state->lock);
        coder_info->data->number_of_compiles[coder_info->thread_id] += 1;
        printf("%d is debugging\n", coder_info->thread_id);
        usleep(coder_info->data->time_to_debug * 1000);
        printf("%d is refactoring\n", coder_info->thread_id);
        usleep(coder_info->data->time_to_refactor * 1000);
    }
    return (NULL);

void *monitor_func(void *monitor_state)
{
    while (1)
        continue;
    return (NULL);
}

int create_coder_thread(pthread_t *threads, int coder_num, shared_data *data,
                        void *(*coder_func)(void*))
{
    int i;
    struct coder_thread *coder_info;
    struct queue *start_queue;
    struct dongle_list *dongles;
    struct manager *state;

    i = -1;
    start_queue = init_queue(coder_num);
    dongle_list = init_dongle_list(coder_num);
    state = init_manager();
    if (!start_queue || !dongle_list || !state)
        return (printf("Error\n", 0));
    while (++i < coder_num)
    {
        coder_info = malloc(sizeof(*coder_info));
        if (!coder_info)
            return (printf("Error\n", 0);
        coder_info->thread_id = i;
        coder_info->number_of_dongles = 0;
        coder_info->data = data;
        coder_info->queue = start_queue;
        coder_info->dongles = dongle_list;
        coder_info->state = state;
        if (pthread_create(&threads[i], NULL, coder_func, coder_info))
            return (printf("Error\n"), 0);
    }
    return (1);
}

int init_threads(pthread_t *threads, int *parsed_args,
                 void *(*coder_func)(void *), void *(*monitor_func)(void *))
{
    int coder_num;
    shared_data *data;
    monitor_thread *monitor_info;

    coder_num = parsed_args[0];
    data = init_data(parsed_args);
    if (!data)
        return (printf("Error\n", 0);
    monitor_info = malloc(sizeof(*monitor_info));
    if (!monitor_info)
        return (printf("Error\n", 0);
    if (!create_coder_thread(threads, coder_num, data, coder_func))
        return (printf("Error\n", 0);
    monitor_info->thead_id = coder_num;
    monitor_info->data = data;
    if (pthread_create(&threads[coder_num], NULL, monitor_func, monitor_info))
        return (printf("Error\n"), 0);
    return (1);
}
