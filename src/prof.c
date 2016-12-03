#include "prof.h"
#include "plat.h"
#if defined(OS_LINUX)
    #define HAVE_POSIX_TIMER
    #include <time.h>
    #ifdef CLOCK_MONOTONIC
        #define CLOCKID CLOCK_MONOTONIC
    #else
        #define CLOCKID CLOCK_REALTIME
    #endif
#elif defined(OS_OSX)
    #define HAVE_MACH_TIMER
    #include <mach/mach_time.h>
#elif defined(OS_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

timepoint_t nanosecs()
{
    static uint64_t is_init = 0;
#if defined(OS_OSX)
    static mach_timebase_info_data_t info;
    if (0 == is_init) {
        mach_timebase_info(&info);
        is_init = 1;
    }
    uint64_t now;
    now = mach_absolute_time();
    now *= info.numer;
    now /= info.denom;
    return now;
#elif defined(OS_LINUX)
    static struct timespec linux_rate;
    if (0 == is_init) {
        clock_getres(CLOCKID, &linux_rate);
        is_init = 1;
    }
    uint64_t now;
    struct timespec spec;
    clock_gettime(CLOCKID, &spec);
    now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
    return now;
#elif defined(OS_WINDOWS)
    static LARGE_INTEGER win_frequency;
    if (0 == is_init) {
        QueryPerformanceFrequency(&win_frequency);
        is_init = 1;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (uint64_t) ((1e9 * now.QuadPart)  / win_frequency.QuadPart);
#endif
}

timepoint_t millisecs()
{
    return nanosecs() / 1.0e6;
}
