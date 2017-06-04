#define SERVER_QUEUE_NAME "/Linda-Server" 
#define QUEUE_PERMISSIONS 0660 
#define MAX_MESSAGES 10 
#define MAX_MSG_SIZE 256
#define PID_SIZEOF sizeof(pid_t)
#define PID 1
#define UINT_SIZEOF sizeof(uint8_t)
#define COMMAND 1
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

#define CORRECT_STATUS 0x0
#define OP_SEND 0x1
#define OP_READ 0x2
#define OP_GET 0x4




struct tuple;
typedef struct tuple tuple;

typedef struct response{
	unsigned code;
	tuple * tuple;
} response;


void open_server_queue(mqd_t * server_q, char * server_name);
pid_t init_client(mqd_t * server_q, mqd_t * client_q, char * server_name);
void send_message(pid_t client_pid, mqd_t server_q, mqd_t client_q);
pid_t run_client(char * server_name, mqd_t * server, mqd_t * client);

void l_output(tuple * message, mqd_t server_q, pid_t c_pid);
response l_read(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid);
response l_input(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid);
