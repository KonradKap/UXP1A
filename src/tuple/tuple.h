#ifndef TUPLE_TUPLE_H
#define TUPLE_TUPLE_H

#define TUPLE_INT_FORMAT 'i'
#define TUPLE_FLOAT_FORMAT 'f'
#define TUPLE_STRING_FORMAT 's'

#define TUPLE_E_OUT_OF_RANGE -1
#define TUPLE_E_INVALID_TYPE -2
#define TUPLE_E_BAD_SIZE -3

struct tuple_element;

typedef struct tuple {
    unsigned nelements;
    struct tuple_element *elements;
} tuple;

tuple *tuple_make(char *format, ...);
void tuple_free(tuple *obj);

int tuple_typeof(tuple *obj, unsigned position);

int tuple_get_int(tuple *obj, unsigned position, int *output);
int tuple_get_float(tuple *obj, unsigned position, float *output);
int tuple_get_string(tuple *obj, unsigned position, char *output);

int tuple_to_buffer(tuple *obj, char *buffer, int size);
tuple *tuple_from_buffer(char *buffer);

#endif //TUPLE_TUPLE_H
