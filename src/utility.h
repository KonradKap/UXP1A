#ifndef UTILITY_H
#define UTILITY_H

#include <unistd.h>
#include <sys/types.h>

#define IGNORED(a) ((void)(a))
#define PTR_SIZE (sizeof(void *))

#define SERVER_QUEUE_NAME "/Linda-Server"
#define CLIENT_NAME_SIZE 64 
#define QUEUE_PERMISSIONS 0660 
#define MAX_MESSAGES 10 
#define MAX_MSG_SIZE 256
#define PID_SIZEOF sizeof(pid_t)
#define PID 1
#define UINT_SIZEOF sizeof(uint8_t)
#define COMMAND 1
#define STATUS 1

#define INCORRECT_STATUS -1
#define CORRECT_STATUS 0x0
#define OP_SEND 0x1
#define OP_READ 0x2
#define OP_GET 0x4
 
typedef struct tuple tuple;

int string_to_int(char *buffer, int *output);
int string_to_float(char *buffer, float *output);
void pack_pid(pid_t pid, char *dest);
pid_t unpack_pid(char *src);
void print_tuple(tuple *obj);
char *error_to_string(int code);

#endif //UTILITY_H
