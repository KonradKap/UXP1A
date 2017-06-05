#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <mqueue.h>

#include "utility.h"
#include "client/client.h"
#include "tuple/tuple.h"
#include "tuple/tuple_element.h"

static void print_help(const char *name);

static int get_request(char *request_str);
static unsigned get_number_of_elements(int argc, char **argv);
static tuple *parse_commandline(unsigned nelements, int blueprint, int argc, char **argv);
static int get_operator(char *argument);
static void do_request(tuple *obj, int request);
static void handle_response(response *received);
static tuple *handle_error(tuple *obj, const char *message);

int main (int argc, char **argv) {
    if (argc < 4) {
        print_help(argv[0]);
        return -1;
    }
    int request = get_request(argv[1]);
    if (request == -1) {
        print_help(argv[0]);
        return -1;
    }
    unsigned nelements = get_number_of_elements(argc, argv);
    if (nelements == 0) {
        print_help(argv[0]);
        return -1;
    }
    tuple *obj = parse_commandline(nelements, request != OP_SEND, argc, argv);
    if (obj == NULL)
        return -1;

    do_request(obj, request);
    tuple_free(obj);
}

static void do_request(tuple *obj, int request) {
    mqd_t server;
    mqd_t client;
    pid_t pid = run_client(SERVER_QUEUE_NAME, &server, &client);
    switch (request) {
        case OP_SEND:
            l_output(obj, server, pid);
            printf("Message sent.\n");
            return;
        case OP_READ: {
                response received = l_read(obj, server, client, pid);
                handle_response(&received);
            }
            return;
        case OP_GET: {
                response received = l_input(obj, server, client, pid);
                handle_response(&received);
            }
            return;
    }

}

static void handle_response(response *received) {
    if (received->code == CORRECT_STATUS) {
        print_tuple(received->tuple);
        tuple_free(received->tuple);
        return;
    }
    printf("%s\n", tuple_error_to_string(received->code));
}

static void print_help(const char *name) {
    printf("Usage:\n");
    printf("%s send|read|get -n [int] [elements]\n", name);
    printf("\tWhere elements is -i|-f|-s [operator] [value]\n");
    printf("\tWhere operator is eq|lt|le|gt|ge|any (only specified if command != send)\n");
    printf("\tWith operator == any value is forbidden\n");
    printf("\n");
}



static unsigned get_number_of_elements(int argc, char **argv) {
    int opt = getopt(argc, argv, "n:");
    if (opt != 'n') {
        fprintf(stderr, "Missing option '-n'.\n");
        return 0;
    }
    int nelements = 0;
    int result = string_to_int(optarg, &nelements);
    if (result != 0) {
        fprintf(stderr, "Invalid argument for option '-n': '%s'.\n", optarg);
    }
    return nelements;
}

static int get_request(char *request_str) {
    if (strcmp(request_str, "send") == 0)
        return OP_SEND;
    if (strcmp(request_str, "read") == 0)
        return OP_READ;
    if (strcmp(request_str, "get") == 0)
        return OP_GET;
    fprintf(stderr, "Invalid command: '%s'.\n", request_str);
    return -1;
}

static int get_operator(char *argument) {
    if (strcmp(argument, "any") == 0)
        return OP_ANY;
    if (strcmp(argument, "eq") == 0)
        return OP_EQ;
    if (strcmp(argument, "lt") == 0)
        return OP_LT;
    if (strcmp(argument, "le") == 0)
        return OP_LE;
    if (strcmp(argument, "gt") == 0)
        return OP_GT;
    if (strcmp(argument, "ge") == 0)
        return OP_GE;
    return OP_BAD;
}

#define read_commandline(obj, index, blueprint, type)\
    do { \
        type value = 0; \
        int operator = OP_ANY; \
        if (blueprint) { \
            operator = get_operator(optarg); \
            if (operator == OP_BAD) { \
                return handle_error(obj, "Invalid value for operator"); \
            } \
        } \
        if (operator != OP_ANY || !blueprint) { \
            char *arg = blueprint ? optarg : argv[optind]; \
            int result = string_to_##type(arg, &value); \
            if (result != 0) \
                return handle_error(obj, "Invalid value for tuple"); \
        } \
        tuple_set_##type##_op((obj), (index)++, value, operator); \
    } while (0)

static tuple *handle_error(tuple *obj, const char *message) {
    fprintf(stderr, "%s: '%s',\n", message, optarg);
    tuple_free(obj);
    return NULL;
}

static tuple *parse_commandline(unsigned nelements, int blueprint, int argc, char **argv) {
    tuple *obj = tuple_make_nelements(nelements);
    unsigned index = 0;
    int opt = -1;
    while ((opt = getopt(argc, argv, "i:f:s:")) != -1) {
        switch(opt) {
            case 'i':
                read_commandline(obj, index, blueprint, int);
                break;
            case 'f':
                read_commandline(obj, index, blueprint, float);
                break;
            case 's':
                if (blueprint) {
                    int operator = get_operator(optarg);
                    if (operator == OP_BAD)
                        return handle_error(obj, "Invalid value for operator");
                    tuple_set_string_op(obj, index++, "", operator);
                }
                else
                    tuple_set_string(obj, index++, optarg);

                break;
            case '?':
            default:
                tuple_free(obj);
                return NULL;
        }
    }
    if (index != nelements) {
        fprintf(stderr, "Invalid number of fields for tuple, got %u, expected %u.\n",
            index,
            nelements);
        tuple_free(obj);
        return NULL;
    }

    return obj;
}
