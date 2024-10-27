#include "network.h"

// Initialize server
int init_server(NetworkConnection* server) {
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd < 0) {
        perror("Server: Socket creation failed");
        return -1;
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(SERVER_PORT);

    if (bind(server->socket_fd, (struct sockaddr*)&server->address, sizeof(server->address)) < 0) {
        perror("Server: Binding failed");
        close(server->socket_fd);
        return -1;
    }

    if (listen(server->socket_fd, MAX_PLAYERS) < 0) {
        perror("Server: Listen failed");
        close(server->socket_fd);
        return -1;
    }
    printf("Server is listening on port %d\n", SERVER_PORT);
    return 0;
}

// Initialize client
int init_client(NetworkConnection* client, const char* server_ip) {
    client->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket_fd < 0) {
        perror("Client: Socket creation failed");
        return -1;
    }

    client->address.sin_family = AF_INET;
    client->address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, server_ip, &client->address.sin_addr) <= 0) {
        perror("Client: Invalid address/Address not supported");
        return -1;
    }

    if (connect(client->socket_fd, (struct sockaddr*)&client->address, sizeof(client->address)) < 0) {
        perror("Client: Connection failed");
        return -1;
    }
    printf("Connected to server at %s:%d\n", server_ip, SERVER_PORT);
    return 0;
}

// Send message
int send_message(NetworkConnection* connection, const char* message) {
    int len = send(connection->socket_fd, message, strlen(message), 0);
    if (len < 0) {
        perror("Send failed");
    }
    return len;
}

// Receive message
int receive_message(NetworkConnection* connection, char* buffer, int buffer_size) {
    int len = recv(connection->socket_fd, buffer, buffer_size - 1, 0);
    if (len < 0) {
        perror("Receive failed");
        return -1;
    }
    buffer[len] = '\0'; // Null-terminate the received string
    return len;
}

// Close connection
void close_connection(NetworkConnection* connection) {
    close(connection->socket_fd);
    printf("Connection closed\n");
}
