#include "tuple/tuple.h"

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tuple/tuple_element.h"
#include "utility.h"

static unsigned parse_format(va_list *list, tuple *obj, char format, int index) {
    switch (format) {
        case TUPLE_INT_FORMAT:
            obj->elements[index].type = INT_TYPE;
            obj->elements[index].data.i = va_arg(*list, int);
            return 5;
        case TUPLE_FLOAT_FORMAT:
            obj->elements[index].type = FLOAT_TYPE;
            obj->elements[index].data.f = va_arg(*list, double);
            return 5;
        case TUPLE_STRING_FORMAT: {
            obj->elements[index].type = STRING_TYPE;
            char *string = va_arg(*list, char *);
            obj->elements[index].data.s.length = strlen(string);
            obj->elements[index].data.s.string = malloc(obj->elements[index].data.s.length + 1);
            strcpy(obj->elements[index].data.s.string, string);
            return obj->elements[index].data.s.length + 2 + PTR_SIZE;
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

tuple *tuple_make(char *format, ...) {
    va_list list;
    va_start(list, format);
    unsigned size;
    tuple *obj = malloc(sizeof(tuple));
    obj->nelements = count_size(format);
    obj->elements = malloc(sizeof(tuple_element) * obj->nelements);
    char c, i = 0;
    while((c = *format++)) {
        unsigned part_size = parse_format(&list, obj, c, i);
        if (part_size != 0)
            ++i;
        size += part_size;
        assert(size <= TUPLE_MAX_SIZE);
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
