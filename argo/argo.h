#ifndef ARGO_H
# define ARGO_H

# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>

typedef struct json {
    enum {
        MAP,
        INTEGER,
        STRING
    } type;
    union {
        struct {
            struct pair *data;
            size_t      size;
        } map;
        int integer;
        char    *string;
    };
}   json;

typedef struct pair {
    char    *key;
    json    value;
}   pair;

#endif