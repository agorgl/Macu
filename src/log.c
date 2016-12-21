#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* -------------------------------------------------- */
/* Helpers                                            */
/* -------------------------------------------------- */
static char* log_type_to_string(enum log_type type)
{
    switch (type)
    {
#ifdef LOG_TYPE_CAPS
        case INFO:  return "INFO";
        case DEBUG: return "DEBUG";
        case WARN:  return "WARNING";
        case ERR:   return "ERROR";
#else
        case INFO:  return "Info";
        case DEBUG: return "Debug";
        case WARN:  return "Warning";
        case ERR:   return "Error";
#endif
        default: return "???";
    }
}

static char* log_bake(const char* msg, enum log_type type)
{
    /* Convert type to string */
    char* type_str = log_type_to_string(type);

    /* Get lengths */
    size_t msg_length = strlen(msg);
    size_t type_length = strlen(type_str);

    /* Calculate total output string length */
    size_t length = type_length + 2 + msg_length;

    /* Allocate space for log */
    char* log = (char*) malloc(length);
    memset(log, 0, length);

    /* Construct log */
    strncpy(log, type_str, type_length);
    strncat(log, " ", 1);
    strncat(log, msg, msg_length);

    return log;
}

static void log_print(const char* msg, enum log_type type)
{
    /* Choose output stream */
    FILE* out = (type == INFO || type == DEBUG) ? stdout : stderr;

    /* Create log */
    char* log = log_bake(msg, type);

    /* Append newline to log */
    log = realloc(log, strlen(log) + 1 /* newline */ + 1 /* terminating char */);
    strncat(log, "\n", 1);

    /* Print */
    fputs(log, out);

    /* Free allocated space for log after printing */
    free(log);
}

/* -------------------------------------------------- */
/* Header implementation                              */
/* -------------------------------------------------- */
void log_info(const char* msg)  { log_print(msg, INFO);  }
void log_debug(const char* msg) { log_print(msg, DEBUG); }
void log_warn(const char* msg)  { log_print(msg, WARN);  }
void log_err(const char* msg)   { log_print(msg, ERR);   }
