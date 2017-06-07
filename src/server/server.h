#include <stdint.h>
#include <mqueue.h> 

#define SERVER_QUEUE_NAME "/Linda-Server"

typedef struct tuple tuple;
struct waiting_proces{
	pid_t c_pid;
	uint8_t command;
	tuple * pattern;
} waiting_proces;


void init_server(char * server_name, mqd_t * server, struct mq_attr * attr);
void run_server(char * server_name);
void recive_message(mqd_t  server);
uint8_t get_command(char * src);
void add_process(uint8_t command, pid_t c_pid, tuple * pattern );
void add_tuple(tuple * pattern);
int return_tuple_index_for_pattern(tuple * pattern);
tuple * get_tuple(int index, int command);
void send_tuple_to_client(tuple * obj, pid_t c_pid, int command);
void remove_tuple(int index);
void update_process_queue();

void close_server(mqd_t server, char * server_queue_name);
void send_reply(pid_t c_pid, int reply);
