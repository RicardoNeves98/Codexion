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

int *parsing(char **argv)
{
    int i;
    int num;
    int *parsed_args;

    i = -1;
    parsed_args = malloc(8 * sizeof(*parsed_args));
    if (!parsed_args)
        return (NULL);
    while (++i < 8)
    {
        num = atoi(argv[i]);
        if (num <= 0)
            return (display_error(argv, i));
        else:
            parsed_args[i] = num;
    }
    if (strcmp(argv[8], "fifo") == 0)
        parsed_args[8] = 0;
    else if (strcmp(argv[8], "edf") != 0)
        parsed_args[8] = 1;
    else
        display_error(argv, 8);
    return (parsed_args);
}

