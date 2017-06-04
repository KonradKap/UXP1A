#ifndef UTILITY_H
#define UTILITY_H

#include <unistd.h>
#include <sys/types.h>

#define IGNORED(a) ((void)(a))
#define PTR_SIZE (sizeof(void *))

int string_to_int(char *buffer, int *output);
int string_to_float(char *buffer, float *output);
void pack_pid(pid_t pid, char *dest);
pid_t unpack_pid(char *src);

#endif //UTILITY_H
