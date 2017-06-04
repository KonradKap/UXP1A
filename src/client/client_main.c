#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <mqueue.h>

#include "utility.h"
#include "client/client.h"
#include "tuple/tuple.h"
#include "tuple/tuple_element.h"

static void print_help();
static void print_tuple(tuple *obj);
static int get_request(char *request_str);
static unsigned get_number_of_elements(int argc, char **argv);
static tuple *parse_commandline_tuple(unsigned nelements, int argc, char **argv);
static int get_operator(char *argument);
static tuple *parse_commandline_blueprint(unsigned nelements, int argc, char **argv);
static void do_request(tuple *obj, int request);
static void handle_response(response *received);

int main (int argc, char **argv) {
    if (argc < 4) {
        print_help();
        return -1;
    }
    int request = get_request(argv[1]);
    if (request == -1) {
        print_help();
        return -1;
    }
    unsigned nelements = get_number_of_elements(argc, argv);
    if (nelements == 0) {
        print_help();
        return -1;
    }
    tuple *obj = NULL;
    if (request == OP_SEND)
        obj = parse_commandline_tuple(nelements, argc, argv);
    else
        obj = parse_commandline_blueprint(nelements, argc, argv);
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

static void print_help() {
    printf("Hello world\n");
}

static void print_tuple(tuple *obj) {
    printf("Tuple:\n  [ ");
    for (unsigned i = 0; i < obj->nelements; ++i) {
        switch(tuple_typeof(obj, i)) {
            case INT_TYPE: {
                    int value = 0;
                    tuple_get_int(obj, i, &value);
                    printf("i:%d ", value);
                }
                break;
            case FLOAT_TYPE: {
                    float value = 0;
                    tuple_get_float(obj, i, &value);
                    printf("f:%f ", value);
                }
                break;
            case STRING_TYPE: {
                    char *value = NULL;
                    tuple_get_string(obj, i, &value);
                    printf("s:%s ", value);
                }
                break;
        }
    }
    printf("]\n");
}

static unsigned get_number_of_elements(int argc, char **argv) {
    int opt = getopt(argc, argv, "n:");
    if (opt != 'n') {
        fprintf(stderr, "Missing option '-n'\n");
        return 0;
    }
    int nelements = 0;
    int result = string_to_int(optarg, &nelements);
    if (result != 0) {
        fprintf(stderr, "Invalid argument for option '-n': '%s'\n", optarg);
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
    fprintf(stderr, "Invalid command: '%s'\n", request_str);
    return -1;
}

static tuple *parse_commandline_tuple(unsigned nelements, int argc, char **argv) {
    tuple *obj = tuple_make_nelements(nelements);
    unsigned index = 0;
    int opt = -1;
    while ((opt = getopt(argc, argv, "i:f:s:")) != -1) {
        switch(opt) {
            case 'i': {
                    int value = 0;
                    int result = string_to_int(optarg, &value);
                    if (result != 0) {
                        fprintf(stderr, "Invalid value for integer: '%s'", optarg);
                        tuple_free(obj);
                        return NULL;
                    }
                    tuple_set_int(obj, index++, value);
                }
                break;
            case 'f': {
                    float value = 0;
                    int result = string_to_float(optarg, &value);
                    if (result != 0) {
                        fprintf(stderr, "Invalid value for float: '%s'", optarg);
                        tuple_free(obj);
                        return NULL;
                    }
                    tuple_set_float(obj, index++, value);
                }
                break;
            case 's': {
                    tuple_set_string(obj, index++, optarg);
                }
                break;
            case '?':
            default:
                tuple_free(obj);
                return NULL;
        }
    }
    return obj;
}

static int get_operator(char *argument) {
    if (strcmp(argument, "any"))
        return OP_ANY;
    if (strcmp(argument, "eq"))
        return OP_EQ;
    if (strcmp(argument, "lt"))
        return OP_LT;
    if (strcmp(argument, "le"))
        return OP_LE;
    if (strcmp(argument, "gt"))
        return OP_GT;
    if (strcmp(argument, "ge"))
        return OP_GE;
    return OP_BAD;
}

static tuple *parse_commandline_blueprint(unsigned nelements, int argc, char **argv) {
    tuple *obj = tuple_make_nelements(nelements);
    unsigned index = 0;
    int opt = -1;
    while ((opt = getopt(argc, argv, "i:f:s:")) != -1) {
        switch(opt) {
            case 'i': {
                    int value = 0;
                    int operator = get_operator(optarg);
                    if (operator == OP_BAD) {
                        fprintf(stderr, "Invalid value for operator: '%s'", optarg);
                        tuple_free(obj);
                        return NULL;
                    }
                    if (operator != OP_ANY) {
                        int result = string_to_int(argv[optind], &value);
                        if (result != 0) {
                            fprintf(stderr, "Invalid value for integer: '%s'", optarg);
                            tuple_free(obj);
                            return NULL;
                        }
                    }
                    tuple_set_int_op(obj, index++, value, operator);
                }
                break;
            case 'f': {
                    float value = 0;
                    int operator = get_operator(optarg);
                    if (operator == OP_BAD) {
                        fprintf(stderr, "Invalid value for operator: '%s'", optarg);
                        tuple_free(obj);
                        return NULL;
                    }
                    if (operator != OP_ANY) {
                        int result = string_to_float(argv[optind], &value);
                        if (result != 0) {
                            fprintf(stderr, "Invalid value for float: '%s'", optarg);
                            tuple_free(obj);
                            return NULL;
                        }
                    }
                    tuple_set_float_op(obj, index++, value, operator);
                }
                break;
            case 's': {
                    int operator = get_operator(optarg);
                    if (operator == OP_BAD) {
                        fprintf(stderr, "Invalid value for operator: '%s'", optarg);
                        tuple_free(obj);
                        return NULL;
                    }
                    tuple_set_string_op(obj, index++, argv[optind], operator);
                }
                break;
            case '?':
            default:
                tuple_free(obj);
                return NULL;
        }
    }
    return obj;
}
