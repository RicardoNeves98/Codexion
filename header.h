#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <stdlib.h>

typedef struct coder
{
    int coder_num;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles;
}   coder;

typedef struct monitor
{
    int dongle_cooldown;
    int number_of_compiles_required;
    struct coder    *coder_info;
}   monitor;
