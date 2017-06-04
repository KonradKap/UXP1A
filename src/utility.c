#include "utility.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int string_to_int(char *buffer, int *output) {
    char *end = buffer;
    int temp = strtol(buffer, &end, 10);
    if (*end != '\0')
        return -1;
    *output = temp;
    return 0;
}

int string_to_float(char *buffer, float *output) {
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
    uint8_t size = *((uint8_t *)src);
    src += sizeof(uint8_t);
    pid_t pid = *((pid_t *)src);

    printf("unpacked Pid size: %d\n", size);
    printf("Unpacked Pid: %d\n", pid);

    return pid;
}

