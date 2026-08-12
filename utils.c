#include "codexion.h"

int *get_zeros_arr(int size)
{
    int i;
    int *arr;

    i = 0;
    arr = malloc(size * sizeof(*arr));
    if (!arr)
        return (NULL);
    while (i < size)
        arr[i++] = 0;
    return (arr);
}

void switch_dongles(struct dongle **curr_dongle, struct dongle **next_dongle)
{
    dongle *pass_dongle;

    pass_dongle = *curr_dongle;
    *curr_dongle = *next_dongle;
    *next_dongle = pass_dongle;
}

