#include "codexion.h"

void display_error(char **argv, int i)
{
    char *args[8] = {
        "number_of_coders", "time_to_burnout", "time_to_compile",
        "time_to_debug", "time_to_refactor", "number_of_compiles_required",
        "dongle_cooldown", "scheduler"
    }
    if (i <= 7)
        printf("[ERROR] '%s' should be a positiver integer\n", args[i])
    if (i == 8)
        printf("[ERROR] '%s' should be either 'fifo' or 'edf'\n", args[i])
}

int parsing_args(char **argv, int *arr)
{
    int i;
    int num;

    i = -1;
    while (++i <= 8)
    {
        num = atoi(argv[i]);
        if (num <= 0)
        {
            display_error(argv, i);
            return (free(arr), 0);
        }
        else:
            arr[i] = num;
    }
    if (strcmp(argv[i], "fifo") == 0)
        arr[i] = 0;
    else if (strcmp(argv[i], "edf") == 0)
        arr[i] = 1;
    else
    {
        display_error(argv, i);
        return (free(arr), 0);
    }
    return (1);
}

int main(int argc, char **argv)
{
    int *arr;

    if (argc != 9)
    {
        printf("[ERROR] Requires 9 arguments to run\n");
        return (1);
    }
    arr = malloc(8 * sizeof(int));
    if (!arr)
    {
        printf("[ERROR] Allocation of memory failed\n");
        return (1);
    }
    if (!parsing_args(arr))
        return (1);
}
