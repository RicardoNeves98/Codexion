#include "codexion.h"

void fill_data(int *parsed_args, struct shared_data *data)
{
    data->coder_num = parsed_args[0];
    data->time_to_compile = parsed_args[2];
    data->time_to_debug = parsed_args[3];
    data->time_to_refactor = parsed_args[4];
    data->comp_required = parsed_args[5];
    data->cooldown = parsed_args[6];
    data->scheduler = parsed_args[7];
    data->active = 1;
    data->total_comp = 0;
    data->time_to_burnout = ms_to_timespec(parsed_args[1]);
    data->max_wait = ms_to_timespec(parsed_args[2] + parsed_args[6]);
    free(parsed_args);
}

int init_data_mutex_and_cond(struct shared_data *data)
{
    if (pthread_mutex_init(&data->output_mutex, NULL))
    {
        printf("Error initializing mutex variable\n");
        return (0);
    }
    if (pthread_mutex_init(&data->queue_mutex, NULL))
    {
        printf("Error initializing mutex variable\n");
        pthread_mutex_destroy(&data->output_mutex);
        return (0);
    }
    if (pthread_cond_init(&data->queue_cond, NULL))
    {
        printf("Error initializing cond variable\n");
        pthread_mutex_destroy(&data->output_mutex);
        pthread_mutex_destroy(&data->queue_mutex);
        return (0);
    }
    return (1);
}

struct shared_data *init_data(int *parsed_args)
{
    struct timespec now;
    struct queue *deadline;
    struct shared_data *data;

    if (!parsed_args)
        return (NULL);
    data = malloc(sizeof(*data));
    if (!data)
        return (free(parsed_args), NULL);
    fill_data(parsed_args, data);
    deadline = init_deadline(data->coder_num);
    if (!deadline)
        return (free(data), NULL);
    if (!init_data_mutex_and_cond(data))
        return (free(deadline), free(data), NULL);
    clock_gettime(CLOCK_MONOTONIC, &now);
    data->start_time = now;
    data->deadline = deadline;
    return (data);
} 
