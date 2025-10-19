#ifndef ERROR_H
#define ERROR_H

typedef enum {
    ERR_FAILED = -1,
    ERR_SUCCESS,
    ERR_NOT_FOUND,
    ERR_IO,
    ERR_FORMAT,
    ERR_UNKNOWN
} error_t;

#endif // ERROR_H