// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef ZKTC3_H
#define ZKTC3_H

//#define ZKT_DEBUG_MSG

#include <inttypes.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ZKT_START_BYTE 0xAA
#define ZKT_STOP_BYTE 0x55
#define ZKT_VERSION1 0x01

#define ZKT_CMD_CONNECT 0x76
#define ZKT_CMD_DISCONNECT 0x02
#define ZKT_CMD_DEVICE_CONTROL 0x05
#define ZKT_CMD_GET_REALTIME_INFO 0x0B
#define ZKT_CMD_ACK 0xC8

#define ZKT_OUTPUT_DOOR 0x01
#define ZKT_OUTPUT_AUX 0x02

#define ZKT_STATUS_DISCONNECTED 0x00
#define ZKT_STATUS_CONNECTED 0x01


typedef struct zkt_connection{
    char *dev_type;
    char *ip;
    uint16_t port;
    uint16_t ssid;
    uint16_t reply_counter;
    uint8_t status;
    int socket_desc;        //Sockets
} zkt_connection_t;

extern void printhex(char *data, size_t length, int space, int newline);
extern uint16_t crc16_arc(uint8_t *data, size_t len);
extern int zkt_connect(zkt_connection_t *connection);
extern int zkt_disconnect(zkt_connection_t *connection);
extern int zkt_get_events(zkt_connection_t *connection, char **reply);
extern uint32_t zkt_get_wiegand34_id(char *reply, size_t reply_length);
extern int zkt_open_door(zkt_connection_t *connection, uint8_t output_type,
                         uint8_t output_number, uint8_t duration);

#endif // ZKTC3_H
