// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef NETWORK_H
#define NETWORK_H

//#define NETWORK_DEBUG_MSG

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int tcp_connect(int *socket_desc, char *server_ip, uint16_t port);
int tcp_disconnect(int socket_desc);
int tcp_send(char *send_data, size_t send_data_size, int socket_desc);
int tcp_recv(char **recv_data, int socket_desc);

#endif // NETWORK_H
