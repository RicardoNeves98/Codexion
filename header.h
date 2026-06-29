#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <stdlib.h>

typedef struct s_list
{
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int nb_of_comp;
    int nb_of_comp_required;
}   t_list;

typedef struct monitor
{
    int dongle_cooldown;
    struct coder    *coder_info;
}   monitor;
