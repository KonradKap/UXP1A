#include <sys/types.h>
#include <mqueue.h>

struct tuple;
typedef struct tuple tuple;

typedef struct response{
	unsigned code;
	tuple * tuple;
} response;

pid_t init_client(mqd_t * server_q, mqd_t * client_q, char * server_name);
void close_client(mqd_t * server, mqd_t * client, pid_t pid);
void open_server_queue(mqd_t * server_q, char * server_name);
pid_t run_client(char * server_name, mqd_t * server, mqd_t * client);


void l_output(tuple * message, mqd_t server_q, mqd_t client_q, pid_t c_pid);
response l_read(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid);
response l_input(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid);
