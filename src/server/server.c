/* * server.c: Server program * to demonstrate interprocess commnuication * with POSIX message queues */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdint.h>
#include <signal.h>

#include "utility.h"
#include "server/server.h"
#include "tuple/tuple.h"
#include "error.h"

tuple * messages[MAX_MESSAGES];
int current_m = 0;
struct waiting_proces queue[MAX_MESSAGES];
int current_item = 0;

static volatile int running = 1;

void interrupt_handler(int dummy) {
    IGNORED(dummy);
    running = 0;
}

void init_server(char * server_name, mqd_t * server, struct mq_attr * attr){
    printf("Server: Init. \n");

    attr->mq_flags = 0;
    attr->mq_maxmsg = MAX_MESSAGES;
    attr->mq_msgsize = MAX_MSG_SIZE;
    attr->mq_curmsgs = 0;

    if((*(server) = mq_open(server_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, attr)) == -1){
    	perror ("Server: mq_open (server)");
        exit (1);
    };

    signal(SIGINT, interrupt_handler);
}

void run_server(char * server_name){
	mqd_t server;
	struct mq_attr attr;

    init_server(server_name, &server,  &attr);
    printf("Server: Run. \n");
    recive_message(server);
}

void recive_message(mqd_t  server){
    char in_buffer [MAX_MSG_SIZE];

    while (running) {
        if (mq_receive(server, in_buffer, MAX_MSG_SIZE, NULL) == -1) {
            perror("mq_receive");
            break;
        }
	printf("Server: message received.\n");

        pid_t pid_c = unpack_pid(in_buffer);
		uint8_t command = get_command(in_buffer);
		int offset = 2* sizeof(uint8_t) + sizeof(pid_t);
		tuple * message = tuple_from_buffer(in_buffer + offset);

		printf("Command: %d Pid: %d\nRecived tuple: ", command, pid_c);
		if(message != NULL)
			print_tuple(message);

		switch(command){
			case OP_SEND:
				if(current_m >= MAX_MESSAGES){
					send_reply(pid_c, SERVER_E_OVERLOAD_MESSAGE_BUFFER);
					continue;
				}
				add_tuple(message);
				send_reply(pid_c, CORRECT_STATUS);
				break;
			case OP_READ:
				if(current_item >= MAX_MESSAGES){
					send_reply(pid_c, SERVER_E_OVERLOAD_CLIENT_QUEUE);
					continue;
				}
				add_process(command, pid_c, message);
				break;
			case OP_GET:
				if(current_item >= MAX_MESSAGES){
					send_reply(pid_c, SERVER_E_OVERLOAD_CLIENT_QUEUE);
					continue;
				}
				add_process(command, pid_c, message);
				break;
			default:
				break;
		}
		for(int i = 0; i < current_item; i++){
			if(queue[i].c_pid < 0)
				continue;
			int index = return_tuple_index_for_pattern(queue[i].pattern);
			if(index > -1){
				tuple * response = get_tuple(index, queue[i].command);
				send_tuple_to_client(response, queue[i].c_pid, queue[i].command);
				queue[i].c_pid = -1;
			}
		}
		update_process_queue();
    }
    printf("Closing resources.\n");
    close_server(server, SERVER_QUEUE_NAME);
}

uint8_t get_command(char * src){
	src += sizeof(uint8_t);
	src += sizeof(pid_t);
	return *((uint8_t *)src);
}

void add_process(uint8_t command, pid_t c_pid, tuple * pattern ){
	if(current_item < MAX_MESSAGES ){
	struct waiting_proces elem;
	elem.c_pid = c_pid;
	elem.command = command;
	elem.pattern = pattern;

	queue[current_item] = elem;
	current_item+=1;
	printf("Add process: Current item: %d\n", current_item);
	}
}

void add_tuple(tuple * pattern){
	if(current_m < MAX_MESSAGES ){
		messages[current_m] = pattern;
		current_m +=1;
		printf("Count tuples: %d\n", current_m);
	}
}

int return_tuple_index_for_pattern(tuple * pattern){
	for(int i = 0; i< current_m; i++){
		if(tuple_compare_to(messages[i], pattern)){
			return i;
		}
	}
	return -1;
}

tuple * get_tuple(int index, int command){
	tuple * temp = messages[index];
	if(command == OP_GET){
		messages[index] = messages[current_m-1];
		current_m -= 1;
		printf("Get tuple: %d\n", current_m);
	}
	return temp;
}

void send_tuple_to_client(tuple * obj, pid_t c_pid, int command){
	mqd_t client;

	char client_name[CLIENT_NAME_SIZE];
	char out_buffer[MAX_MSG_SIZE];
	sprintf (client_name, "/%d", c_pid);
	uint8_t count;
	printf("Sending tuple: ");
	if(obj != NULL)
		print_tuple(obj);
	count = tuple_to_buffer(obj, out_buffer + STATUS, MAX_MSG_SIZE - STATUS);

	if(count == obj->nelements){
		*((uint8_t *)out_buffer) = CORRECT_STATUS;
	}
	else{
		*((uint8_t *)out_buffer) = INCORRECT_STATUS;
	}

	if(command == OP_GET){
		tuple_free(obj);
	}
	client = mq_open (client_name, O_WRONLY);

    if (client == -1) {
        perror ("Server: Not able to open client queue");
    }
    if (mq_send (client, out_buffer, MAX_MSG_SIZE, 0) == -1) {
        perror ("Server: Not able to send message to client");
    }
}

void remove_tuple(int index){
	--current_m;
	tuple * temp = messages[index];
	if(current_m && index != current_m ){
		messages[index] = messages[current_m];
		printf("Current_m: %d\n", current_m);
	}
	tuple_free(temp);
}

void update_process_queue(){
	if(current_item!=0){
		int temp_index = 0;

		for(int i = 0; i<current_item; ++i){
			if(queue[i].c_pid < 0){
				if(i == current_item -1){
					--current_item;
					return;
				}
				temp_index = i;
				while(temp_index < current_item){
					queue[temp_index]= queue[temp_index+1];
					++temp_index;
				}
				--current_item;
			}
		}
	}
}

static void free_tuples_queue(){
	for (int i = 0; i < current_m; ++i){
		tuple_free(messages[i]);
	}
}

void close_server(mqd_t server, char * server_queue_name){
	free_tuples_queue();

    if (mq_close (server) == -1) {
        perror ("Server: mq_close");
        exit (1);
    }
    if (mq_unlink (server_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
}

void send_reply(pid_t c_pid, int reply){
	mqd_t client;

	char client_name[CLIENT_NAME_SIZE];
	char out_buffer[MAX_MSG_SIZE];
	sprintf (client_name, "/%d", c_pid);
	printf("Sending  reply: %d \n", reply);
	*((uint8_t *)out_buffer) = reply;
	
	client = mq_open (client_name, O_WRONLY);

    if (client == -1) {
        perror ("Server: Not able to open client queue");
    }
    if (mq_send (client, out_buffer, MAX_MSG_SIZE, 0) == -1) {
        perror ("Server: Not able to send message to client");
    }
}




