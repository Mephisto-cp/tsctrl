// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "network.h"

int tcp_connect(int *socket_desc, char *server_ip, uint16_t port)
{
    int result;
    struct sockaddr_in server;
    struct timeval timeout;

    *socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    #ifdef NETWORK_DEBUG_MSG
    if (*socket_desc == -1){
        printf("[ERROR] tcp_connect: Could not create socket\n");
        return -1;
    } else {
        printf("[  OK  ] tcp_connect: Socket created successfully\n");
    }
    #else
    if ( *socket_desc == -1 )
        return -1;
    #endif

    //Set recv() and send() timeout for 3 seconds
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    setsockopt(*socket_desc, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(*socket_desc, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    result = connect(*socket_desc, (struct sockaddr *)&server, sizeof(server));
    #ifdef NETWORK_DEBUG_MSG
    if ( result < 0){
        printf("[ERROR] tcp_connect: Unable to establish connection\n");
        close(*socket_desc);
        return result;
    } else {
        printf("[  OK  ] tcp_connect: Connection established successfully\n");
    }
    #else
    if ( result < 0){
        close(*socket_desc);
        return result;
    }
    #endif

    return 0;
}

int tcp_disconnect(int socket_desc)
{
    shutdown(socket_desc, 2);
    close(socket_desc);

    return 0;
}

int tcp_send(char *send_data, size_t send_data_size, int socket_desc)
{
    int result;

    result = send(socket_desc, send_data, send_data_size, 0);
    #ifdef NETWORK_DEBUG_MSG
    if (result < 0) {
        printf("[ERROR] tcp_send: Failed to send data\n");
    } else {
        printf("[  OK  ] tcp_send: Data sent successfully\n");
    }
    #endif

    return result;
}

int tcp_recv(char **recv_data, int socket_desc)
{
    int result;
    char recv_buffer[2000];

    (*recv_data) = 0;
    result = recv(socket_desc, recv_buffer, sizeof(recv_buffer), 0);
    if(result > 0){
        (*recv_data) = malloc(result);
        if((*recv_data)!=0){
            memcpy((*recv_data), recv_buffer, result);
        }
        #ifdef NETWORK_DEBUG_MSG
        printf("[  OK  ] tcp_recv: Received %d bytes of data\n", result);
        #endif
    }
    else{
        #ifdef NETWORK_DEBUG_MSG
        printf("[  Error  ] tcp_recv: Somthing went wrong (%d)\n", result);
        #endif
    }

    return result;
}

