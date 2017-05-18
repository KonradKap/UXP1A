#include "tuple/tuple.h"

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tuple/tuple_element.h"
#include "utility.h"

#define write_to_buffer(what, buffer, size, type)\
    do{\
        if ((size) < (int)(sizeof((what))))\
            return TUPLE_E_BAD_SIZE;\
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
            obj->elements[index].data.s.length = strlen(string);
            obj->elements[index].data.s.string = malloc(obj->elements[index].data.s.length + 1);
            strcpy(obj->elements[index].data.s.string, string);
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
            free(obj->elements[i].data.s.string);
    }
    free(obj->elements);
    free(obj);
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
    return obj;
}

void tuple_free(tuple *obj) {
    rollback_free(obj, obj->nelements);
}

int tuple_typeof(const tuple *obj, unsigned position) {
    if (obj->nelements <= position)
        return TUPLE_E_OUT_OF_RANGE;
    return obj->elements[position].type;
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
        strcpy(output, obj->elements[position].data.s.string);
    return is_valid;
}

int tuple_set_int(tuple *obj, unsigned position, int input) {
    int type = tuple_typeof(obj, position);
    if (type < 0)
        return type;
    if ((type & TYPE_MASK) == STRING_TYPE)
        free(obj->elements[position].data.s.string);
    obj->elements[position].type = INT_TYPE;
    obj->elements[position].data.i = input;
    return 0;
}

int tuple_set_float(tuple *obj, unsigned position, float input) {
    int type = tuple_typeof(obj, position);
    if (type < 0)
        return type;
    if ((type & TYPE_MASK) == STRING_TYPE)
        free(obj->elements[position].data.s.string);
    obj->elements[position].type = FLOAT_TYPE;
    obj->elements[position].data.i = input;
    return 0;
}

int tuple_set_string(tuple *obj, unsigned position, char *input) {
    int type = tuple_typeof(obj, position);
    if (type < 0)
        return type;
    obj->elements[position].data.s.length = strlen(input);
    if ((type & TYPE_MASK) == STRING_TYPE)
        obj->elements[position].data.s.string =
            realloc(obj->elements[position].data.s.string, obj->elements[position].data.s.length + 1);
    else
        obj->elements[position].data.s.string = malloc(obj->elements[position].data.s.length + 1);

    obj->elements[position].type = STRING_TYPE;
    strcpy(obj->elements[position].data.s.string, input);
    return 0;
}

int tuple_set_int_op(tuple *obj, unsigned position, int input, unsigned short operator) {
    return 0;
}

int tuple_set_float_op(tuple *obj, unsigned position, float input, unsigned short operator) {
    return 0;
}

int tuple_set_string_op(tuple *obj, unsigned position, char *input, unsigned short operator) {
    return 0;
}

int tuple_compare_to(const tuple* obj, const tuple *blueprint) {
    return 0;
}

int tuple_to_buffer(const tuple *obj, char *buffer, int size) {
    write_to_buffer(obj->nelements, buffer, size, unsigned);
    for (unsigned i = 0; i < obj->nelements && size > 0; ++i) {
        write_to_buffer(obj->elements[i].type, buffer, size, unsigned short);
        switch (obj->elements[i].type & TYPE_MASK) {
            case INT_TYPE:
                write_to_buffer(obj->elements[i].data.i, buffer, size, int);
                break;
            case FLOAT_TYPE:
                write_to_buffer(obj->elements[i].data.f, buffer, size, float);
                break;
            case STRING_TYPE:
                if (size < (int)obj->elements[i].data.s.length)
                    return TUPLE_E_BAD_SIZE;
                strcpy(buffer, obj->elements[i].data.s.string);
                buffer += obj->elements[i].data.s.length + 1;
                size -= obj->elements[i].data.s.length;
                break;
            default:
                return TUPLE_E_INVALID_TYPE;
        }
    }
    return 0;
}

tuple *tuple_from_buffer(const char *buffer) {
    tuple *obj = malloc(sizeof(tuple));
    read_from_buffer(obj->nelements, buffer, size, unsigned);
    obj->elements = malloc(sizeof(tuple_element) * obj->nelements);
    for (unsigned i = 0; i < obj->nelements; ++i) {
        read_from_buffer(obj->elements[i].type, buffer, size, unsigned short);
        switch(obj->elements[i].type & TYPE_MASK) {
            case INT_TYPE:
                read_from_buffer(obj->elements[i].data.i, buffer, size, int);
                break;
            case FLOAT_TYPE:
                read_from_buffer(obj->elements[i].data.f, buffer, size, float);
                break;
            case STRING_TYPE:
                obj->elements[i].data.s.length = strlen(buffer);
                obj->elements[i].data.s.string = malloc(obj->elements[i].data.s.length + 1);
                strcpy(obj->elements[i].data.s.string, buffer);
                buffer += obj->elements[i].data.s.length + 1;
                break;
            default:
                rollback_free(obj, i);
                return NULL;
        }
    }
    return obj;
}
