#ifndef TUPLE_TUPLE_ELEMENT_H
#define TUPLE_TUPLE_ELEMENT_H

#include <stdint.h>

typedef struct tuple_element {
    uint16_t type;
    union {
        int i;
        float f;
        char *s;
    } data;
} tuple_element;

#endif //TUPLE_TUPLE_ELEMENT_H
