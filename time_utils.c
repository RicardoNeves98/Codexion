#include "codexion.h"

struct timespec ms_to_timespec(int time_ms)
{
    struct timespec new_format;

    new_format.tv_sec = time_ms / 1000;
    new_format.tv_nsec = (time_ms % 1000) * 1000000;
    return (new_format);
}

int timespec_to_ms(struct timespec time)
{
    int time_ms;

    time_ms = time.tv_sec * 1000;
    time_ms += time.tv_nsec / 1000000;
    return (time_ms);
}

int get_time_diff(struct timespec time1, struct timespec time2)
{
    return (nano_to_ms(time1) - nano_to_ms(time2)); 
}

struct timespec get_min_time(struct timespec time1, struct timespec time2)
{
    if (get_time_diff(time1, time2) < 0)
        return (time1);
    return (time2);
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

struct timespec add_curr_time(struct timespec time)
{
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    return (add_time(now, time));
}
