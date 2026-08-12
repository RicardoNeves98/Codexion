#include "codexion.h"

struct timespec time_convert(int time_ms)
{
    struct timespec new_format;

    new_format.tv_sec = time_ms / 1000;
    new_format.tv_nsec = (time_ms % 1000) * 1000000;
    return (new_format);
}

// This function returns 1 if TIME1 is OLDER or SAME as TIME2 and 0 in the other case
int cmp_time(struct timespec time1, struct timespec time2)
{
    if (time1.tv_sec < time2.tv_sec)
        return (1);
    else if (time1.tv_sec == time2.tv_sec)
        if (time1.tv_nsec <= time2.tv_nsec)
            return (1);
    return (0);
}

struct timespec add_time(struct timespec time1, struct timespec time2)
{
    struct timespec result;
    long total_nano;

    total_nano = time1.tv_nsec + time2.tv_nsec;
    result.tv_nsec = total_nano % 1000000000;
    result.tv_sec = time1.tv_sec + time2.tv_sec + (total_nano / 1000000000);
    return (result);
}

void update_next_aval(dongle *curr_dongle)
{
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    curr_dongle->next_aval = add_time(now, curr_dongle->cooldown);
}
