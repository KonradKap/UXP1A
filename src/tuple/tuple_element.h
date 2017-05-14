#ifndef TUPLE_TUPLE_ELEMENT_H
#define TUPLE_TUPLE_ELEMENT_H

#define INT_TYPE 0x02
#define FLOAT_TYPE 0x01
#define STRING_TYPE 0x00

typedef struct tuple_element {
    unsigned char type;
    union {
        int i;
        float f;
        struct {
            unsigned char length;
            char *string;
        } s;
    } data;
} tuple_element;

#endif //TUPLE_TUPLE_ELEMENT_H
