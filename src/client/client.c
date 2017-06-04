/* * client.c: Client program * to demonstrate interprocess commnuication * with POSIX message queues */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include "utility.h"
#include "client/client.h"
#include "tuple/tuple.h"

pid_t init_client(mqd_t * server_q, mqd_t * client_q, char * server_name){

	struct mq_attr attr;
	char client_queue_name [64];
	pid_t pid = getpid();
	printf("Client pid: %d\n", pid);

    sprintf (client_queue_name, "/%d", pid);

	attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    //printf("%s \n", client_queue_name);
    printf("%s\n", "Init client.\n" );
    if ((*(client_q) = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }

    open_server_queue(server_q, server_name);

    return pid;
}



void open_server_queue(mqd_t * server_q, char * server_name){
    printf("Open server queue\n");
	if ((*(server_q) = mq_open (server_name, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
}


pid_t run_client(char * server_name, mqd_t * server, mqd_t * client){


	pid_t c_pid = init_client(server, client, server_name);

	return c_pid;
    //send_message(c_pid, server_q, client_q);
}

void send_message(pid_t c_pid, mqd_t server_q, mqd_t client_q){

    char in_buffer [MSG_BUFFER_SIZE];
    char send_buffer [MSG_BUFFER_SIZE];
    char  client_queue_name [64];

    sprintf (client_queue_name, "/%d", c_pid);

    //pack32(c_pid, send_buffer);
    pack_pid(c_pid, send_buffer);
    int free_index = sizeof(pid_t) + PID;
    printf("Free index %d\n",free_index );
    send_buffer[free_index ] = (OP_READ & 0x000000ff);
    send_buffer[free_index + 1] = 'a';
    pid_t temp_pid = unpack_pid(send_buffer);
    printf("Pid: %d\n", temp_pid );

    //printf("Comand: %d\n", (OP_READ & 0x000000ff) );

    printf("Message: %c Command: %d pid_c: %d\n", send_buffer[free_index +1 ], send_buffer[free_index], c_pid);

    printf ("Ask for a token (Press <ENTER>): ");
    char temp_buf [10];
    int temp = 1;

    while (fgets (temp_buf, 2, stdin)) {
        // send message to server
    	printf("Client before send message\n");
        send_buffer[free_index ] = (OP_SEND & 0x000000ff);
    	send_buffer[free_index  + 1] = 'b';

    	if(temp ==1){
			send_buffer[free_index ] = (OP_GET & 0x000000ff);
			send_buffer[free_index  + 1] = 'b';
			//printf("Comand2: %d\n", (OP_SEND & 0x000000ff) );

    	}
    	if(temp ==2){
			send_buffer[free_index ] = (OP_GET & 0x000000ff);
			printf(" comand 2");
			send_buffer[free_index  + 1] = 'b';
			//printf("Comand2: %d\n", (OP_SEND & 0x000000ff) );
    	}



    	temp+=1;



    	printf("Send buffor: %ld\n", sizeof(send_buffer));
    	//printf("hello");


        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
            continue;

        }
        // receive response from server
        printf("Recive from server:\n");
        if (mq_receive (client_q, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);

        } // display token received from server
        printf ("Client: Token received from server: %s\n\n", in_buffer);
        printf ("Ask for a token (Press ): ");
    }


    if (mq_close (client_q) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (client_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    printf ("Client: bye\n");

    exit (0);
}

static int prepare_header(char * buffer, pid_t c_pid, int command){

    pack_pid(c_pid, buffer);
    //pid_t temp = unpack_pid(send_buffer);
    int free_index = PID_SIZEOF + PID;
    int header_length = free_index + 1;
    buffer[free_index ] = (OP_SEND & 0x000000ff);

    return header_length;


}


void l_output(tuple * message, mqd_t server_q, pid_t c_pid){
	char send_buffer[MAX_MSG_SIZE];

    //pack_pid(c_pid, send_buffer);
    //pid_t temp = unpack_pid(send_buffer);
    //int free_index = PID_SIZEOF + PID;
    //int header = free_index + 1;
    //send_buffer[free_index ] = (OP_SEND & 0x000000ff);
    int offset = prepare_header(send_buffer, c_pid, OP_SEND);
    int status = tuple_to_buffer(message, send_buffer + offset, MAX_MSG_SIZE - offset);

    //pid_t temp_pid = unpac_pid(send_buffer);
    printf(" status %d\n", status);

    if(status == 0){
        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
        }
    }
}



response l_read(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid){
    char send_buffer[MAX_MSG_SIZE];
    char recive_buffer[MAX_MSG_SIZE];

    response message;
    message.code = -1;
    message.tuple = NULL;

    int offset = prepare_header(send_buffer, c_pid, OP_READ);
    int status = tuple_to_buffer(pattern, send_buffer + offset, MAX_MSG_SIZE - offset);

    if(status == 0){
        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
        }

    printf("Recive from server:\n");
        if (mq_receive (client_q, recive_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        status = *((uint8_t *)recive_buffer);
        if(status == CORRECT_STATUS){
            response message;
            tuple * recived = tuple_from_buffer(recive_buffer + COMMAND);
            message.tuple = recived;
            message.code = status;
        }

    }
    return message;
}

response l_input(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid){
    char send_buffer[MAX_MSG_SIZE];
    char recive_buffer[MAX_MSG_SIZE];
    response message;
    message.code = -1;
    message.tuple = NULL;



    int offset = prepare_header(send_buffer, c_pid, OP_GET);
    int status = tuple_to_buffer(pattern, send_buffer + offset, MAX_MSG_SIZE - offset);

    if(status == CORRECT_STATUS){
        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
        }

        printf("Recive from server:\n");
        if (mq_receive (client_q, recive_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        status = *((uint8_t *)recive_buffer);
        if(status == CORRECT_STATUS){
            response message;
            tuple * recived = tuple_from_buffer(recive_buffer + COMMAND);
            message.tuple = recived;
            message.code = status;
        }
    }
    message;

}
