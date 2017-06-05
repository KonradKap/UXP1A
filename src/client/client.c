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

    sprintf (client_queue_name, "/%d", pid);

	attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    printf("%s\n", "Init client.\n" );
    if ((*(client_q) = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }
    open_server_queue(server_q, server_name);
    return pid;
}

void close_client(mqd_t * server, mqd_t * client, pid_t c_pid){
    char client_queue_name [64];
    sprintf (client_queue_name, "/%d", c_pid);

    if (mq_close (*(client)) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }
    if (mq_close (*(server)) == -1) {
        perror ("Server: mq_close");
        exit (1);
    }
    if (mq_unlink (client_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
}

void open_server_queue(mqd_t * server_q, char * server_name){
	if ((*(server_q) = mq_open (server_name, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
}

pid_t run_client(char * server_name, mqd_t * server, mqd_t * client){
	pid_t c_pid = init_client(server, client, server_name);
	return c_pid;
}

static int prepare_header(char * buffer, pid_t c_pid, int command){
    pack_pid(c_pid, buffer);
    //pid_t temp = unpack_pid(send_buffer);
    int free_index = PID_SIZEOF + PID;
    int header_length = free_index + 1;
    buffer[free_index ] = (command & 0x000000ff);
    return header_length;
}

void l_output(tuple * message, mqd_t server_q, pid_t c_pid){
	char send_buffer[MAX_MSG_SIZE];

    int offset = prepare_header(send_buffer, c_pid, OP_SEND);
    unsigned status = tuple_to_buffer(message, send_buffer + offset, MAX_MSG_SIZE - offset);

    if(status == message->nelements){
        printf("Sending tuple: \n");
        print_tuple(message);
        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
        }
    }
}

response l_read(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid){
    char send_buffer[MAX_MSG_SIZE];
    char recive_buffer[MAX_MSG_SIZE];

    response message;
    message.code = INCORRECT_STATUS;
    message.tuple = NULL;

    int offset = prepare_header(send_buffer, c_pid, OP_READ);
    uint8_t count = tuple_to_buffer(pattern, send_buffer + offset, MAX_MSG_SIZE - offset);

    if(count == pattern->nelements){
        printf("Sending tuple: \n");
        if(pattern != NULL)
            print_tuple(pattern);
        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
        }
        printf("Recive from server:\n");
        if (mq_receive (client_q, recive_buffer, MAX_MSG_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        count = *((uint8_t *)recive_buffer);
        if(count == CORRECT_STATUS){
            tuple * recived = tuple_from_buffer(recive_buffer + COMMAND);
            if(recived != NULL){
                print_tuple(pattern);
                message.tuple = recived;
                message.code = count;
            }
        }
    }
    return message;
}

response l_input(tuple * pattern, mqd_t server_q, mqd_t client_q, pid_t c_pid){
    char send_buffer[MAX_MSG_SIZE];
    char recive_buffer[MAX_MSG_SIZE];

    response message;
    message.code = INCORRECT_STATUS;
    message.tuple = NULL;

    int offset = prepare_header(send_buffer, c_pid, OP_GET);
    uint8_t count = tuple_to_buffer(pattern, send_buffer + offset, MAX_MSG_SIZE - offset);

    if(count == pattern->nelements){
        printf("Sending tuple: \n");

        if(pattern != NULL)
            print_tuple(pattern);    
        if (mq_send (server_q, send_buffer, MAX_MSG_SIZE, 0) == -1) {
            perror ("Client: Not able to send message to server");
        }

        printf("Recive from server:\n");

        if (mq_receive (client_q, recive_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        count = *((uint8_t *)recive_buffer);
        if(count == CORRECT_STATUS){
            tuple * recived = tuple_from_buffer(recive_buffer + COMMAND);
            if(recived != NULL){
                message.tuple = recived;
                message.code = count;
            }
        }
    }
    return message;
}
