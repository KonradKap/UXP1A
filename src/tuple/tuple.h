#ifndef TUPLE_TUPLE_H
#define TUPLE_TUPLE_H

#define INT_TYPE 0x02
#define FLOAT_TYPE 0x01
#define STRING_TYPE 0x00
#define TYPE_MASK 0x03

#define OP_ANY 0x00
#define OP_EQ 0x04
#define OP_LT 0x08
#define OP_LE 0x10
#define OP_GT 0x20
#define OP_GR 0x40
#define OP_MASK 0xFC

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

tuple *tuple_make(const char *format, ...);
tuple *tuple_make_nelements(unsigned nelements);
void tuple_free(tuple *obj);

int tuple_typeof(const tuple *obj, unsigned position);

int tuple_get_int(const tuple *obj, unsigned position, int *output);
int tuple_get_float(const tuple *obj, unsigned position, float *output);
int tuple_get_string(const tuple *obj, unsigned position, char *output);

int tuple_set_int(tuple *obj, unsigned position, int input);
int tuple_set_float(tuple *obj, unsigned position, float input);
int tuple_set_string(tuple *obj, unsigned position, char *input);

int tuple_set_int_op(tuple *obj, unsigned position, int input, unsigned short operator);
int tuple_set_float_op(tuple *obj, unsigned position, float input, unsigned short operator);
int tuple_set_string_op(tuple *obj, unsigned position, char *input, unsigned short operator);

int tuple_compare_to(const tuple* obj, const tuple *blueprint);

int tuple_to_buffer(const tuple *obj, char *buffer, int size);
tuple *tuple_from_buffer(const char *buffer);

#endif //TUPLE_TUPLE_H
