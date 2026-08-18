#include "codexion.h"

void fill_data(int *parsed_args, struct shared_data *data)
{
    data->coder_num = parsed_args[0];
    data->time_to_compile = parsed_args[2];
    data->time_to_debug = parsed_args[3];
    data->time_to_refactor = parsed_args[4];
    data->compiles_required = parsed_args[5];
    data->cooldown = parsed_args[6];
    data->scheduler = parsed_args[7];
    data->time_to_burnout = time_convert(parsed_args[1]);
    data->max_wait = ms_to_timespec(parsed_args[2] + parsed_args[6]);
}

int init_data_mutex_and_cond(struct shared_data *data)
{
    if (pthread_mutex_init(&data->output, NULL))
    {
        printf("Error initializing mutex variable\n");
        return (0);
    }
    if (pthread_mutex_init(&data->state, NULL))
    {
        printf("Error initializing mutex variable\n");
        pthread_mutex_destroy(&data->output);
        return (0);
    }
    if (pthread_cond_init(&data->cond, NULL))
    {
        printf("Error initializing cond variable\n");
        pthread_mutex_destroy(&data->output);
        pthread_mutex_destroy(&data->state);
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
    deadline = malloc(sizeof(*deadline));
    if (!deadline)
        return (free(parsed_args), NULL);
    data = malloc(sizeof(*data));
    if (!data)
        return (free(parsed_args), free(deadline), NULL);
    if (!init_data_mutex_and_cond(data))
        return (free(parsed_args), free(deadline), NULL);
    clock_gettime(CLOCK_MONOTONIC, &now);
    data->start_time = now;
    data->deadline = deadline;
    fill_data(parsed_args, data);
    return (free(parsed_args), data);
}
