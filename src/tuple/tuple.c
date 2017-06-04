#include "tuple/tuple.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tuple/tuple_element.h"
#include "utility.h"

#define write_to_buffer(what, buffer, buffer_begin, size, current, type)\
    do{\
        if ((size) < (int)(sizeof((what)))) {\
            *(buffer_begin) = (current);\
            return (current);\
        }\
        *(type *)(buffer) = (what);\
        (buffer) += sizeof((what));\
        (size) -= sizeof((what));\
    } while(0)

#define read_from_buffer(what, buffer, size, type)\
    do {\
        (what) = *(type *)(buffer);\
        (buffer) += sizeof((what));\
    } while (0)

static unsigned parse_format(va_list *list, tuple *obj, char format, int index) {
    switch (format) {
        case TUPLE_INT_FORMAT:
            obj->elements[index].type = INT_TYPE;
            obj->elements[index].data.i = va_arg(*list, int);
            return 1;
        case TUPLE_FLOAT_FORMAT:
            obj->elements[index].type = FLOAT_TYPE;
            obj->elements[index].data.f = va_arg(*list, double);
            return 1;
        case TUPLE_STRING_FORMAT: {
            obj->elements[index].type = STRING_TYPE;
            char *string = va_arg(*list, char *);
            obj->elements[index].data.s = malloc(strlen(string) + 1);
            strcpy(obj->elements[index].data.s, string);
            return 1;
        }
        default:
            return 0;
    }
}

static unsigned count_size(const char *format) {
    unsigned ret = 0;
    char c;
    while((c = *format++)) {
        if (c == TUPLE_INT_FORMAT
            || c == TUPLE_FLOAT_FORMAT
            || c == TUPLE_STRING_FORMAT)
            ++ret;
    }
    return ret;
}

static int check_validity(const tuple *obj, unsigned position, int type) {
    if (obj->nelements <= position)
        return TUPLE_E_OUT_OF_RANGE;
    if ((obj->elements[position].type & TYPE_MASK) != (type & TYPE_MASK))
        return TUPLE_E_INVALID_TYPE;
    return 0;
}

static void rollback_free(tuple *obj, unsigned index) {
    for (unsigned i = 0; i < index; ++i) {
        if ((obj->elements[i].type & TYPE_MASK) == STRING_TYPE)
            free(obj->elements[i].data.s);
    }
    free(obj->elements);
    free(obj);
}

static int type(const tuple* obj, unsigned position) {
    if (obj->nelements <= position)
        return TUPLE_E_OUT_OF_RANGE;
    return obj->elements[position].type;
}

static int cmp_eq(const tuple_element *lhs, const tuple_element *rhs) {
    switch(lhs->type & TYPE_MASK) {
        case INT_TYPE:
            return lhs->data.i == rhs->data.i;
        case FLOAT_TYPE:
            return fabs(lhs->data.f - rhs->data.f) < EPSILON;
        case STRING_TYPE:
            return strcmp(lhs->data.s, rhs->data.s) == 0;
        default:
            return TUPLE_E_INVALID_TYPE;
    }
}

static int cmp_lt(const tuple_element *lhs, const tuple_element *rhs) {
    switch(lhs->type & TYPE_MASK) {
        case INT_TYPE:
            return lhs->data.i < rhs->data.i;
        case FLOAT_TYPE:
            return lhs->data.f < rhs->data.f;
        case STRING_TYPE:
            return strcmp(lhs->data.s, rhs->data.s) == -1;
        default:
            return TUPLE_E_INVALID_TYPE;
    }
}

static int operator_fit(const tuple_element *obj, const tuple_element *blueprint) {
    switch (blueprint->type & OP_MASK) {
        case OP_ANY:
            return 1;
        case OP_EQ:
            return cmp_eq(obj, blueprint);
        case OP_LT: //obj < blueprint
            return cmp_lt(obj, blueprint);
        case OP_LE: // obj <= blueprint <=> !(blueprint < obj)
            return !cmp_lt(blueprint, obj);
        case OP_GT: // obj > blueprint <=> blueprint < obj
            return cmp_lt(blueprint, obj);
        case OP_GE: // obj >= blueprint <=> !(obj < blueprint)
            return !cmp_lt(obj, blueprint);
        default:
            return TUPLE_E_INVALID_OP;
    }
}

tuple *tuple_make(const char *format, ...) {
    va_list list;
    va_start(list, format);
    tuple *obj = tuple_make_nelements(count_size(format));
    char c, i = 0;
    while((c = *format++)) {
        i += parse_format(&list, obj, c, i);
    }
    return obj;
}

tuple *tuple_make_nelements(unsigned nelements) {
    tuple *obj = malloc(sizeof(tuple));
    obj->nelements = nelements;
    obj->elements = malloc(sizeof(tuple_element) * obj->nelements);
    for (unsigned i = 0; i < nelements; ++i) {
        obj->elements[i].type = INT_TYPE;
    }
    return obj;
}

void tuple_free(tuple *obj) {
    rollback_free(obj, obj->nelements);
}

int tuple_typeof(const tuple *obj, unsigned position) {
    int result = type(obj, position);
    return result < 0
        ? result
        : result & TYPE_MASK;
}

int tuple_operator(const tuple *obj, unsigned position) {
    int result = type(obj, position);
    return result < 0
        ? result
        : result & OP_MASK;
}

int tuple_get_int(const tuple *obj, unsigned position, int *output) {
    int is_valid = check_validity(obj, position, INT_TYPE);
    if (is_valid == 0)
        *output = obj->elements[position].data.i;
    return is_valid;
}

int tuple_get_float(const tuple *obj, unsigned position, float *output) {
    int is_valid = check_validity(obj, position, FLOAT_TYPE);
    if (is_valid == 0)
        *output = obj->elements[position].data.f;
    return is_valid;
}

int tuple_get_string(const tuple *obj, unsigned position, char *output) {
    int is_valid = check_validity(obj, position, STRING_TYPE);
    if (is_valid == 0)
        strcpy(output, obj->elements[position].data.s);
    return is_valid;
}

int tuple_set_int(tuple *obj, unsigned position, int input) {
    int type = tuple_typeof(obj, position);
    if (type < 0)
        return type;
    if ((type & TYPE_MASK) == STRING_TYPE) {
        free(obj->elements[position].data.s);
    }
    obj->elements[position].type = INT_TYPE;
    obj->elements[position].data.i = input;
    return 0;
}

int tuple_set_float(tuple *obj, unsigned position, float input) {
    int type = tuple_typeof(obj, position);
    if (type < 0)
        return type;
    if ((type & TYPE_MASK) == STRING_TYPE)
        free(obj->elements[position].data.s);
    obj->elements[position].type = FLOAT_TYPE;
    obj->elements[position].data.f = input;
    return 0;
}

int tuple_set_string(tuple *obj, unsigned position, char *input) {
    int type = tuple_typeof(obj, position);
    if (type < 0)
        return type;
    if ((type & TYPE_MASK) == STRING_TYPE)
        obj->elements[position].data.s =
            realloc(obj->elements[position].data.s,
                    strlen(input) + 1);
    else
        obj->elements[position].data.s =
            malloc(strlen(input));

    obj->elements[position].type = STRING_TYPE;
    strcpy(obj->elements[position].data.s, input);
    return 0;
}

int tuple_set_int_op(tuple *obj, unsigned position, int input, uint16_t operator) {
    int result = tuple_set_int(obj, position, input);
    if (result != 0)
        return result;
    obj->elements[position].type |= operator;
    return 0;
}

int tuple_set_float_op(tuple *obj, unsigned position, float input, uint16_t operator) {
    int result = tuple_set_float(obj, position, input);
    if (result != 0)
        return result;
    obj->elements[position].type |= operator;
    return 0;
}

int tuple_set_string_op(tuple *obj, unsigned position, char *input, uint16_t operator) {
    int result = operator == OP_ANY
        ? tuple_set_string(obj, position, "")
        : tuple_set_string(obj, position, input);

    if (result != 0)
        return result;
    obj->elements[position].type |= operator;
    return 0;
}

int tuple_compare_to(const tuple* obj, const tuple *blueprint) {
    if (blueprint->nelements != obj->nelements)
        return 0;
    for (unsigned i = 0; i < obj->nelements; ++i) {
        if ((obj->elements[i].type & TYPE_MASK) != (blueprint->elements[i].type & TYPE_MASK))
            return 0;
        int cmp_result = operator_fit(&obj->elements[i], &blueprint->elements[i]);
        if (cmp_result <= 0)
            return cmp_result;
    }
    return 1;
}

int tuple_to_buffer(const tuple *obj, char *buffer, int size) {
    unsigned *buffer_begin = (unsigned *)buffer;
    write_to_buffer(obj->nelements, buffer, buffer_begin, size, 0, unsigned);
    for (unsigned i = 0; i < obj->nelements; ++i) {
        write_to_buffer(obj->elements[i].type, buffer, buffer_begin, size, i, uint16_t);
        switch (obj->elements[i].type & TYPE_MASK) {
            case INT_TYPE:
                write_to_buffer(obj->elements[i].data.i, buffer, buffer_begin, size, i, int);
                break;
            case FLOAT_TYPE:
                write_to_buffer(obj->elements[i].data.f, buffer, buffer_begin, size, i, float);
                break;
            case STRING_TYPE: {
                    int length = strlen(obj->elements[i].data.s);
                    if (size < length + 1) {
                        *buffer_begin = i;
                        return i;
                    }
                    strcpy((char *)buffer, obj->elements[i].data.s);
                    buffer += length + 1;
                    size -= length + 1;
                }
                break;
            default:
                return TUPLE_E_INVALID_TYPE;
        }
    }
    return obj->nelements;
}

tuple *tuple_from_buffer(const char *buffer) {
    tuple *obj = malloc(sizeof(tuple));
    read_from_buffer(obj->nelements, buffer, size, unsigned);
    obj->elements = malloc(sizeof(tuple_element) * obj->nelements);
    for (unsigned i = 0; i < obj->nelements; ++i) {
        read_from_buffer(obj->elements[i].type, buffer, size, uint16_t);
        switch(obj->elements[i].type & TYPE_MASK) {
            case INT_TYPE:
                read_from_buffer(obj->elements[i].data.i, buffer, size, int);
                break;
            case FLOAT_TYPE:
                read_from_buffer(obj->elements[i].data.f, buffer, size, float);
                break;
            case STRING_TYPE: {
                    int length = strlen(buffer);
                    obj->elements[i].data.s = malloc(length + 1);
                    strcpy(obj->elements[i].data.s, buffer);
                    buffer += length + 1;
                }
                break;
            default:
                rollback_free(obj, i);
                return NULL;
        }
    }
    return obj;
}
