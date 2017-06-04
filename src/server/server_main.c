#include <stdio.h> 

#include "server/server.h"

int main (int argc, char **argv) {
    printf ("Server: Hello, World!\n");
    run_serwer(SERVER_QUEUE_NAME);
}

