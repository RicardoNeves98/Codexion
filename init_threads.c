#include "codexion.h"

void get_dongles(coder_thread *coder_info, dongle **dongle1, dongle **dongle2)
{
    if (coder_info->id % 2 == 0)
    {
        *dongle1 = coder_info->right_dongle;
        *dongle2 = coder_info->left_dongle;
    }
    else
    {
        *dongle1 = coder_info->left_dongle;
        *dongle2 = coder_info->right_dongle;
    }
}

void start_compile(coder_thread *coder_info, dongle *dongle1, dongle *dongle2)
{
    printf("%d is compiling\n", coder_info->id);
    usleep(coder_info->data->time_to_compile * 1000);
    pthread_mutex_lock(&coder_info->state->wait);
    dongle1->is_free = 1;
    pthread_mutex_unlock(&dongle1->lock);
    dongle2->is_free = 1;
    pthread_mutex_unlock(&dongle2->lock);
    pthread_cond_signal(&coder_info->state->available);
    pthread_mutex_unlock(&coder_info->state->wait);
    coder_info->data->number_of_compiles[coder_info->id] += 1;
    printf("%d is debugging\n", coder_info->id);
    usleep(coder_info->data->time_to_debug * 1000);
    printf("%d is refactoring\n", coder_info->id);
    usleep(coder_info->data->time_to_refactor * 1000);
}

void *coder_func(void *coder_state)
{
    struct dongle *dongle1;
    struct dongle *dongle2;
    struct coder_thread *coder_info;

    coder_info = (struct coder_thread *)coder_state;
    get_dongles(coder_info, &dongle1, &dongle2);
    while (1)
    {
        pthread_mutex_lock(&dongle1->lock);
        pthread_mutex_lock(&coder_info->state->wait);
        if (dongle2->is_free)
        {
            pthread_mutex_lock(&dongle2->lock);
            printf("%d has taken a dongle\n", coder_info->id);
            dongle1->is_free = 0;
            printf("%d has taken a dongle\n", coder_info->id);
            dongle2->is_free = 0;
            pthread_mutex_unlock(&coder_info->state->wait);
            start_compile(coder_info, dongle1, dongle2);
        }
        else
        {
            while (!dongle2->is_free)
            {
                dongle1->is_free = 1;
                pthread_mutex_unlock(&dongle1->lock);
                pthread_cond_wait(&coder_info->state->available, &coder_info->state->wait);
            }
            pthread_mutex_unlock(&coder_info->state->wait);
        }
    }
    return (NULL);
}

shared_state *init_state(void)
{
    shared_state *state;

    state = malloc(sizeof(*state));
    if (!state)
        return (NULL);
    pthread_mutex_init(&state->wait, NULL);
    pthread_cond_init(&state->available, NULL);
    return (state);
}   

int create_coder_thread(pthread_t *threads, int coder_num, shared_data *data,
                        void *(*coder_func)(void*))
{
    int i;
    struct coder_thread *coder_info;
    struct dongle_list *dongles;
    struct shared_state *state;

    i = -1;
    dongle_list = init_dongle_list(coder_num);
    if (!dongle_list)
        return (printf("Error\n", 0));
    state = init_state();
    if (!state)
        return (printf("Error\n", 0));
    while (++i < coder_num)
    {
        coder_info = malloc(sizeof(*coder_info));
        if (!coder_info)
            return (printf("Error\n", 0);
        coder_info->id = i;
        coder_info->left_dongle = &dongle_list[i];
        coder_info->right_dongle = &dongle_list[((i + 1) % (coder_num - 1))];
        coder_info->data = data;
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
