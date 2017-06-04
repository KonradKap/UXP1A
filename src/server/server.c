/* * server.c: Server program * to demonstrate interprocess commnuication * with POSIX message queues */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <mqueue.h> 
#include <stdint.h>

#include "server/server.h"
#include "tuple/tuple.h"

tuple * messages[MAX_MESSAGES];
int current_m = 0;
struct waiting_proces queue[MAX_MESSAGES];
int current_item = 0;




int main (int argc, char **argv) {

    printf ("Server: Hello, World!\n");
    run_serwer(SERVER_QUEUE_NAME);
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
}
    
void run_serwer(char * server_name){

	mqd_t server, client;
	struct mq_attr attr;

    init_server(server_name, &server,  &attr);
    printf("Server: Run. \n");    
    recaive_message(server, client);
    
}     
    
void recaive_message(mqd_t  server, mqd_t  client){

    char in_buffer [MAX_MSG_SIZE];
    char out_buffer [MAX_MSG_SIZE];
    
    while (1) {
        
        // get the oldest message with highest priority


        
        if ((mq_receive (server, in_buffer, MAX_MSG_SIZE, NULL)) == -1) { 
            perror ("Server: mq_receive"); 
            exit (1); 
        }

        //printf("Server Name: %s\n", in_buffer );

		printf ("Server: message received.\n");

        pid_t pid_c = unpack_pid(in_buffer);

		uint8_t command = get_command(in_buffer);
		int offset = 2* sizeof(uint8_t) + sizeof(pid_t);
		tuple * message = tuple_from_buffer(in_buffer + offset);
		printf("Command: %d pid_c: %d\n", command, pid_c);

		switch(command){
			case OP_SEND:
				add_tuple(message);
				//send_tupple_to_client(in_buffer[6], pid_c);
				break;
			case OP_READ:
				add_process(command, pid_c, message);
				break;

			case OP_GET:
				add_process(command, pid_c, message);
				break;
			default: 
				
				break;


		}


		for(int i = 0; i < current_item; i++){
			if(queue[i].c_pid < 0)
				continue;
			int index = return_tupple_index_for_pattern(queue[i].pattern);
			if(index > -1){
				tuple * tupple = get_tupple(index, queue[i].command);
				send_tupple_to_client(tupple, queue[i].c_pid, queue[i].command);
				queue[i].c_pid = -1;
			}
		}
		update_process_queue();
    } 
    
}

pid_t get_pid_from_buffer(char * buffer){
		return buffer[1] | buffer[0] << 8;
}

void pack_pid(pid_t pid, char *dest){
	*((uint8_t *)dest) = sizeof(pid_t);
	dest += sizeof(uint8_t);
	*((pid_t *)dest) = pid;
}

pid_t unpack_pid(char *src){
	uint8_t size = *((uint8_t *)src);
	src += sizeof(uint8_t);
	pid_t pid = *((pid_t *)src);
	
	printf("unpacked Pid size: %d\n", size);
	printf("Unpacked Pid: %d\n", pid);

    return pid;
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
	}

}

void add_tuple(tuple * pattern){
	if(current_m < MAX_MESSAGES ){
		messages[current_m] = pattern;
		current_m +=1;
		printf("Current messages: %d\n", current_m);
	}
}

int return_tupple_index_for_pattern(tuple * pattern){

	for(int i = 0; i< current_m; i++){
		if(tuple_compare_to(pattern, messages[i]))
			return i;
	}

	return -1;
}

tuple * get_tupple(int index, int command){
	tuple * temp = messages[index];

	if(command == OP_GET){
		messages[index] = messages[current_m];
		current_m -= 1;
	}

	return temp;
}

void send_tupple_to_client(tuple * tupple, pid_t c_pid, int command){

	mqd_t client;

	char client_name[CLIENT_NAME_SIZE];
	char out_buffer[MAX_MSG_SIZE];
	sprintf (client_name, "/%d", c_pid);
	uint8_t status;
	status = tuple_to_buffer(tupple, out_buffer + 1, MAX_MSG_SIZE - 1);

	if(command == OP_GET){
		tuple_free(tupple);
	}

	out_buffer[0] = (status & 0x000000ff);

	client = mq_open (client_name, O_WRONLY);

    if (client == -1) { 
        perror ("Server: Not able to open client queue");
    }             
    //sprintf (out_buffer, "%c", tupple);
   
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