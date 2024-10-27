#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define MAX_PLAYERS 4
#define BUFFER_SIZE 256

typedef struct {
    int socket_fd;                     // Socket file descriptor for communication
    struct sockaddr_in address;        // Socket address structure
} NetworkConnection;

// Initializes a server connection
int init_server(NetworkConnection* server);

// Initializes a client connection
int init_client(NetworkConnection* client, const char* server_ip);

// Sends a message through the socket
int send_message(NetworkConnection* connection, const char* message);

// Receives a message through the socket
int receive_message(NetworkConnection* connection, char* buffer, int buffer_size);

// Closes the network connection
void close_connection(NetworkConnection* connection);

#endif // NETWORK_H
