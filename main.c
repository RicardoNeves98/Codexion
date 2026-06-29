#include "codexion.h"

int *display_error(char **argv, int i)
{
    char *args[8] = {
        "number_of_coders", "time_to_burnout", "time_to_compile",
        "time_to_debug", "time_to_refactor", "number_of_compiles_required",
        "dongle_cooldown", "scheduler"
    };

    if (i < 8)
        printf("[ERROR] '%s' should be a positiver integer\n", args[i]);
    if (i == 8)
        printf("[ERROR] 'scheduler' should be either 'fifo' or 'edf'\n");
    return (NULL);
}

int *parsing_args(char **argv)
{
    int i;
    int num;
    int *arr;

    i = 0;
    arr = malloc(8 * sizeof(*arr));
    if (!arr)
        return (NULL)
    while (i < 8)
    {
        num = atoi(argv[i]);
        if (num <= 0)
            return (display_error(argv, i));
        else:
            arr[i++] = num;
    }
    if (strcmp(argv[8], "fifo") == 0)
        arr[8] = 0;
    else if (strcmp(argv[8], "edf") != 0)
        arr[8] = 1;
    else
        display_error(argv, 8);
    return (arr);
}

int get_coder_info(int *arr, t_list *coder_info)
{
    coder_info->time_to_burnout = arr[1];
    coder_info->time_to_compile = arr[2];
    coder_info->time_to_debug = arr[3];
    coder_info->time_to_refactor = arr[4];
    coder_info->nb_of_comp = arr[5];
    coder_info->nb_of_comp_required = arr[6];
    coder_info->scheduler = arr[7];
}

int main(int argc, char **argv)
{
    int i;
    int *arr;
    t_list *coder_info;
    pthread_t *threads;

    i = -1;
    if (argc != 9)
        return (printf("[ERROR] Needs 9 arguments\n"), 1);
    arr = parsing_args(argv);
    if (!arr)
        return (1);
    coder_info = malloc(sizeof(*coder_info));
    threads = malloc(arr[0] * sizeof(*threads));
    if (!coder_info || !threads)
        return (printf("[ERROR] Memory allocation failed\n"), 1);
    get_coder_info(arr, coder_info)
    while (++i < arr[0])
        if pthread_create(threads[i], NULL, function, coder_info)
            break;
}
