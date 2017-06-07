#include "utility.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "tuple/tuple.h"
#include "error.h"

int string_to_int(char *buffer, int *output) {
    if (buffer == NULL)
        return -1;
    char *end = buffer;
    int temp = strtol(buffer, &end, 10);
    if (*end != '\0')
        return -1;
    *output = temp;
    return 0;
}

int string_to_float(char *buffer, float *output) {
    if (buffer == NULL)
        return -1;
    char *end = buffer;
    float temp = strtof(buffer, &end);
    if (*end != '\0')
        return -1;
    *output = temp;
    return 0;
}

void pack_pid(pid_t pid, char *dest) {
    *((uint8_t *)dest) = sizeof(pid_t);
    dest += sizeof(uint8_t);
    *((pid_t *)dest) = pid;
}

pid_t unpack_pid(char *src) {
    src += sizeof(uint8_t);
    pid_t pid = *((pid_t *)src);
    return pid;
}

void print_tuple(tuple *obj) {
    printf("[ ");
    for (unsigned i = 0; i < obj->nelements; ++i) {
        switch(tuple_typeof(obj, i)) {
            case INT_TYPE: {
                    int value = 0;
                    tuple_get_int(obj, i, &value);
                    printf("i:%d ", value);
                }
                break;
            case FLOAT_TYPE: {
                    float value = 0;
                    tuple_get_float(obj, i, &value);
                    printf("f:%f ", value);
                }
                break;
            case STRING_TYPE: {
                    char *value = NULL;
                    tuple_get_string(obj, i, &value);
                    printf("s:%s ", value);
                }
                break;
        }
    }
    printf("]\n");
}

char *error_to_string(int code) {
    switch (code) {
        case 0:
            return "No error.";
        case TUPLE_E_OUT_OF_RANGE:
            return "Exceeded tuple's size.";
        case TUPLE_E_INVALID_TYPE:
            return "Invalid type.";
        case TUPLE_E_INVALID_OP:
            return "Invalid operator";
        default:
            return "Unknown error.";
    }
}
