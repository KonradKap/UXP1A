#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "utility.h"
#include "client/client.h"
#include "tuple/tuple.h"

static void print_help();
static int get_request(char **argv);
static unsigned get_number_of_elements(int argc, char **argv);
static tuple *parse_commandline(unsigned nelements, int argc, char **argv);

int main (int argc, char **argv) {
    if (argc < 4) {
        print_help();
        return -1;
    }
    int request = get_request(argv);
    unsigned nelements = get_number_of_elements(argc, argv);
    if (nelements == 0 || request == -1) {
        print_help();
        return -1;
    }
    tuple *obj = parse_commandline(nelements, argc, argv);
    printf("%d\n", nelements);
    tuple_free(obj);
}

static void print_help() {
    printf("Hello world\n");
}

static unsigned get_number_of_elements(int argc, char **argv) {
    int opt = getopt(argc, argv, "n:");
    unsigned nelements = 0;
    if (opt != 'n' || !(nelements = atoi(optarg))) {
        fprintf(stderr, "Invalid or missing option '-n'\n");
        return 0;
    }
    return nelements;
}

static int get_request(char **argv) {
    char *request_str = argv[1];
    if (strcmp(request_str, "send") == 0)
        return OP_SEND;
    if (strcmp(request_str, "read") == 0)
        return OP_READ;
    if (strcmp(request_str, "get") == 0)
        return OP_GET;
    fprintf(stderr, "Invalid command: '%s'\n", request_str);
    return -1;
}

static tuple *parse_commandline(unsigned nelements, int argc, char **argv) {
    return tuple_make("s", "Kappa");
}
