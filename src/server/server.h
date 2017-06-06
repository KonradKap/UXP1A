
#define SERVER_QUEUE_NAME "/Linda-Server" 
#define QUEUE_PERMISSIONS 0660 
#define MAX_MESSAGES 10 
#define MAX_MSG_SIZE 256 
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#define CLIENT_NAME_SIZE 64


#define OP_SEND 0x1
#define OP_READ 0x2
#define OP_GET 0x4
#define CORRECT_STATUS 0x0
#define INCORRECT_STATUS -1
#define STATUS 1

#include <stdint.h>
#include <mqueue.h> 


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

void close_server(mqd_t * server, char * server_queue_name);
