
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
struct tuple;
typedef struct tuple tuple;
struct waiting_proces{
	pid_t c_pid;
	uint8_t command;
	tuple * pattern;
} waiting_proces;


void init_server(char * server_name, mqd_t * server, struct mq_attr * attr);
void recaive_message(mqd_t  server, mqd_t  client);
void run_serwer(char * server_name);
pid_t get_pid_from_buffer(char * buffer);
void add_process(uint8_t command, pid_t c_pid, tuple * pattern );
void add_tuple(tuple * pattern);
int return_tupple_index_for_pattern(tuple * pattern);
tuple * get_tupple(int index, int command);
void send_tupple_to_client(tuple * tupple, pid_t c_pid, int command);
void remove_tuple(int index);
void update_process_queue();
void pack_pid(pid_t pid, char *dest);
pid_t unpack_pid(char *src);
uint8_t get_command(char * src);
