#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

enum
{
    LOG_INFO,
    LOG_ERROR,
    LOG_DEBUG,
    LOG_WARNING 
};

#define container_of(ptr,contype,member) \
    ((contype *)((char*)(ptr)-offsetof(contype,member)))

static inline void log_add(int,const char *format,...)
{
    va_list va;

    va_start(va,format);
    vfprintf(stderr,format,va);
    va_end(va);
}
