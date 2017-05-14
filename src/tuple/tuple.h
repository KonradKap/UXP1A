#ifndef TUPLE_TUPLE_H
#define TUPLE_TUPLE_H

#define TUPLE_MAX_SIZE 252

#define TUPLE_INT_FORMAT 'i'
#define TUPLE_FLOAT_FORMAT 'f'
#define TUPLE_STRING_FORMAT 's'

struct tuple_element;

typedef struct tuple {
    unsigned nelements;
    struct tuple_element *elements;
} tuple;

tuple *tuple_make(char *format, ...);
void tuple_free(tuple *obj);

#endif //TUPLE_TUPLE_H
