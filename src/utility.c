#include "utility.h"

#include <stdint.h>
#include <stdio.h>

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

