#ifndef TUPLE_TUPLE_ELEMENT_H
#define TUPLE_TUPLE_ELEMENT_H

typedef struct tuple_element {
    unsigned short type;
    union {
        int i;
        float f;
        struct {
            unsigned int length;
            char *string;
        } s;
    } data;
} tuple_element;

#endif //TUPLE_TUPLE_ELEMENT_H
