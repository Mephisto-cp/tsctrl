// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "zkt-c3.h"
#include "network.h"

void printhex(char *data, size_t length, int space, int newline)
{
    for (size_t i=0; i<length; i++) {
        printf("%02X", (uint8_t) data[i]);
        if (space != 0) printf(" ");
    }
    if (newline != 0) printf("\n");
}

uint16_t crc16_arc(uint8_t *data, size_t len){
  uint16_t crc = 0x0000;
  size_t j;
  int i;
  // Note: 0xA001 is the reflection of 0x8005
  for (j=len; j>0; j--) {
    crc ^= *data++;
    for (i=0; i<8; i++) {
      if (crc & 1) crc = (crc >> 1) ^ 0xA001;
      else crc >>= 1;
    }
  }
  return (crc);
}

int zkt_connect(zkt_connection_t *connection)
{
    uint16_t checksum;
    char packet[12];
    char *reply=0;
    int result;

    packet[0] = ZKT_START_BYTE;
    packet[1] = ZKT_VERSION1;
    packet[2] = ZKT_CMD_CONNECT;
    packet[3] = 0x04;               //Size of payload minor byte (uint16_t little-endian)
    packet[4] = 0x00;               //Size of payload major byte (uint16_t little-endian)
    packet[5] = 0x00;               //Session ID minor byte (uint16_t little-endian)
    packet[6] = 0x00;               //Session ID major byte (uint16_t little-endian)
    packet[7] = 0x00;               //Reply counter minor byte (uint16_t little-endian)
    packet[8] = 0x00;               //Reply counter major byte (uint16_t little-endian)
    packet[9] = 0x00;               //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[10] = 0x00;              //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[11] = ZKT_STOP_BYTE;

    memcpy(&packet[7], &(connection->reply_counter), 2);

    checksum = crc16_arc((uint8_t *) &packet[1],sizeof(packet)-4);
    memcpy(&packet[9], &checksum, 2);

    result = tcp_connect(&(connection->socket_desc), connection->ip, connection->port);
    if (result != 0) {
        printf("Unable to connect. Host %s port %u is not responding.\n", connection->ip,
               connection->port);
        return -1;
    }

    result = tcp_send(packet, sizeof(packet), connection->socket_desc);
    if (result == -1) {
        printf("Unable to connect. Host %s port %u is not responding.\n", connection->ip,
               connection->port);
        return -1;
    }

    result = tcp_recv(&reply, connection->socket_desc);
    if ((result > 0) && ((uint8_t) reply[2] == ZKT_CMD_ACK)) {
        memcpy(&(connection->ssid), &reply[5], 2);              //Store received ssid value
        memcpy(&(connection->reply_counter), &reply[7], 2);     //Store received reply counter value
        connection->status = ZKT_STATUS_CONNECTED;
        if (reply != 0) free(reply);
        printf("Connected to %s port %u. SSID=%04X CNTR=%04X\n", connection->ip,
               connection->port, connection->ssid, connection->reply_counter);
        return 0;
    } else {
        printf("Unable to connect. Host %s port %u is not responding.\n", connection->ip,
               connection->port);
        if (reply != 0) free(reply);
        return -1;
    }
}

int zkt_disconnect(zkt_connection_t *connection)
{
    uint16_t checksum;
    char packet[12];
    int result;

    packet[0] = ZKT_START_BYTE;
    packet[1] = ZKT_VERSION1;
    packet[2] = ZKT_CMD_DISCONNECT;
    packet[3] = 0x04;               //Size of payload minor byte (uint16_t little-endian)
    packet[4] = 0x00;               //Size of payload major byte (uint16_t little-endian)
    packet[5] = 0x00;               //Session ID minor byte (uint16_t little-endian)
    packet[6] = 0x00;               //Session ID major byte (uint16_t little-endian)
    packet[7] = 0x00;               //Reply counter minor byte (uint16_t little-endian)
    packet[8] = 0x00;               //Reply counter major byte (uint16_t little-endian)
    packet[9] = 0x00;               //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[10] = 0x00;              //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[11] = ZKT_STOP_BYTE;

    memcpy(&packet[5], &(connection->ssid), 2);     //Set SSID to current connection's value
    checksum = crc16_arc((uint8_t *) &packet[1],sizeof(packet)-4);
    memcpy(&packet[9], &checksum, 2);
    result = tcp_send(packet, sizeof(packet), connection->socket_desc);
    if (result == -1) {
        printf("Host %s port %u is not responding. Connection closed.\n", connection->ip,
               connection->port);
        tcp_disconnect(connection->socket_desc);
        connection->status = ZKT_STATUS_DISCONNECTED;
    } else {
        printf("Disconnected from %s port %u. SSID=%04X CNTR=%04X\n", connection->ip,
               connection->port, connection->ssid, connection->reply_counter);
        tcp_disconnect(connection->socket_desc);
        connection->status = ZKT_STATUS_DISCONNECTED;
    }
    return result;
}

int zkt_get_events(zkt_connection_t *connection, char **reply)
{
    uint16_t checksum;
    char packet[12];
    int result;


    packet[0] = ZKT_START_BYTE;
    packet[1] = ZKT_VERSION1;
    packet[2] = ZKT_CMD_GET_REALTIME_INFO;
    packet[3] = 0x04;               //Size of payload minor byte (uint16_t little-endian)
    packet[4] = 0x00;               //Size of payload major byte (uint16_t little-endian)
    packet[5] = 0x00;               //Session ID minor byte (uint16_t little-endian)
    packet[6] = 0x00;               //Session ID major byte (uint16_t little-endian)
    packet[7] = 0x00;               //Reply counter minor byte (uint16_t little-endian)
    packet[8] = 0x00;               //Reply counter major byte (uint16_t little-endian)
    packet[9] = 0x00;               //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[10] = 0x00;              //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[11] = ZKT_STOP_BYTE;

    memcpy(&packet[5], &(connection->ssid), 2);
    connection->reply_counter++;
    memcpy(&packet[7], &(connection->reply_counter), 2);
    checksum = crc16_arc((uint8_t *) &packet[1],sizeof(packet)-4);
    memcpy(&packet[9], &checksum, 2);
    result = tcp_send(packet, sizeof(packet), connection->socket_desc);
    if (result == -1) {
        printf("Cloud not send zkt_get_events command to host %s port %u. Network error.\n", connection->ip,
               connection->port);
        return -1;
    }
    result = tcp_recv(reply, connection->socket_desc);
    if ((result > 0) && ((*reply)[2] == (char) ZKT_CMD_ACK)) {
        #ifdef ZKT_DEBUG_MSG
        printf("Received realtime events from %s port %u.\n", connection->ip, connection->port);
        #endif
        return result;
    } else {
        printf("Host %s port %u is not responding. Realtime events are not received.\n", connection->ip,
               connection->port);
        connection->status = ZKT_STATUS_DISCONNECTED;
        if (*reply != 0) free(*reply);
        return -1;
    }
}

uint32_t zkt_get_wiegand34_id(char *reply, size_t reply_length)
{
    uint32_t wg34_id;
    if (reply_length != 28) {
        return 0;
    }
    if (reply[2] != (char) 0xC8) {
        return 0;
    }
    memcpy(&wg34_id, &reply[9], 4);

    return wg34_id;
}

int zkt_open_door(zkt_connection_t *connection, uint8_t output_type, uint8_t output_number, uint8_t duration)
{
    uint16_t checksum;
    char packet[17];
    char *reply;
    int result;

    packet[0] = ZKT_START_BYTE;
    packet[1] = ZKT_VERSION1;
    packet[2] = ZKT_CMD_DEVICE_CONTROL;
    packet[3] = 0x09;               //Size of payload minor byte (uint16_t little-endian)
    packet[4] = 0x00;               //Size of payload major byte (uint16_t little-endian)
    packet[5] = 0x00;               //Session ID minor byte (uint16_t little-endian)
    packet[6] = 0x00;               //Session ID major byte (uint16_t little-endian)
    packet[7] = 0x00;               //Reply counter minor byte (uint16_t little-endian)
    packet[8] = 0x00;               //Reply counter major byte (uint16_t little-endian)
    packet[9] = 0x01;               //Unknown (uint8_t)
    packet[10] = 0x00;              //Output number (uint8_t)
    packet[11] = 0x01;              //Output type: Door=0x01, Aux=0x02 (uint8_t)
    packet[12] = 0x00;              //Duration (0 to close immediately) (uint8_t)
    packet[13] = 0x00;              //Unknown (uint8_t)
    packet[14] = 0x00;              //Checksum crc16/ARC counter minor byte (uint16_t little-endian)
    packet[15] = 0x00;              //Checksum crc16/ARC counter major byte (uint16_t little-endian)
    packet[16] = ZKT_STOP_BYTE;

    memcpy(&packet[5], &(connection->ssid), 2);
    connection->reply_counter++;
    memcpy(&packet[7], &(connection->reply_counter), 2);
    packet[10] = output_number;
    packet[11] = output_type;
    packet[12] = duration;
    checksum = crc16_arc((uint8_t *) &packet[1],sizeof(packet)-4);
    memcpy(&packet[14], &checksum, 2);
    result = tcp_send(packet, sizeof(packet), connection->socket_desc);
    if (result == -1) {
        printf("Cloud not send zkt_open_door command to host %s port %u. Network error.\n", connection->ip,
               connection->port);
        connection->status = ZKT_STATUS_DISCONNECTED;
        return -1;
    }
    result = tcp_recv(&reply, connection->socket_desc);
    if ((result > 0) && (reply[2] == (char) ZKT_CMD_ACK)) {
        printf("Received zkt_open_door reply from %s port %u.\n", connection->ip, connection->port);
        return result;
    } else {
        printf("Host %s port %u is not responding. Could not open the door.\n", connection->ip,
               connection->port);
        if (reply != 0) free(reply);
        connection->status = ZKT_STATUS_DISCONNECTED;
        return -1;
    }

    return 0;
}
