#include "tuple/tuple.h"

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tuple/tuple_element.h"
#include "utility.h"

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

static unsigned count_size(char *format) {
    unsigned ret = 0;
    char c;
    while((c = *format++)) {
        if (c == TUPLE_INT_FORMAT || c == TUPLE_FLOAT_FORMAT || c == TUPLE_STRING_FORMAT)
            ++ret;
    }
    return ret;
}

static int check_validity(tuple *obj, unsigned position, int type) {
    if (obj->nelements <= position)
        return TUPLE_E_OUT_OF_RANGE;
    if (obj->elements[position].type != type)
        return TUPLE_E_INVALID_TYPE;
    return 0;
}

tuple *tuple_make(char *format, ...) {
    va_list list;
    va_start(list, format);
    tuple *obj = malloc(sizeof(tuple));
    obj->nelements = count_size(format);
    obj->elements = malloc(sizeof(tuple_element) * obj->nelements);
    char c, i = 0;
    while((c = *format++)) {
        i += parse_format(&list, obj, c, i);
    }
    return obj;
}

void tuple_free(tuple *obj) {
    for (unsigned i = 0; i < obj->nelements; ++i) {
        if (obj->elements[i].type == STRING_TYPE)
            free(obj->elements[i].data.s.string);
    }
    free(obj->elements);
    free(obj);
}

int tuple_typeof(tuple *obj, unsigned position) {
    if (obj->nelements <= position)
        return TUPLE_E_OUT_OF_RANGE;
    return obj->elements[position].type;
}

int tuple_get_int(tuple *obj, unsigned position, int *output) {
    int is_valid = check_validity(obj, position, INT_TYPE);
    if (is_valid == 0)
        *output = obj->elements[position].data.i;
    return is_valid;
}

int tuple_get_float(tuple *obj, unsigned position, float *output) {
    int is_valid = check_validity(obj, position, FLOAT_TYPE);
    if (is_valid == 0)
        *output = obj->elements[position].data.f;
    return is_valid;
}

int tuple_get_string(tuple *obj, unsigned position, char *output) {
    int is_valid = check_validity(obj, position, STRING_TYPE);
    if (is_valid == 0)
        strcpy(output, obj->elements[position].data.s.string);
    return is_valid;
}

int tuple_to_buffer(tuple *obj, void *buffer, unsigned size) {
    IGNORED(obj);
    IGNORED(buffer);
    IGNORED(size);
    assert(0);
    return -1;
}

tuple *tuple_from_buffer(void *buffer, unsigned size) {
    IGNORED(buffer);
    IGNORED(size);
    assert(0);
    return NULL;
}
