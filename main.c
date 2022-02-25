// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//#define DEBUG_MSG

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include "list.h"
#include "network.h"
#include "zkt-c3.h"
#include "ticketapi.h"

list_t* InitServer(const char *filename);
list_t* InitTurnstiles(const char *filename);
list_t* InitTokens(const char *filename);
list_t* InitHeader(const char *filename);
list_t* AppendTokens(list_t *header, list_t *tokens);

list_t* InitServer(const char *filename)
{
    list_t *server;

    server = ListCreateFromTextfile(filename, 0);
    if (server == NULL) {
        printf("Error loading %s file.\n", filename);
        exit(0);
    }
    return server;
}

list_t* InitTurnstiles(const char *filename)
{
    list_t *turnstiles;
    char *tempchar;

    turnstiles = ListCreateFromTextfile(filename, 0);
    if (turnstiles == NULL) {
        printf("Error loading %s file.\n", filename);
        exit(0);
    } else { //Remove finishing /n symbol from each string
        for (int i=0; i<turnstiles->length; i++) {
            tempchar = ListIndexData(turnstiles, i);
            tempchar[(strlen(tempchar)-1)] = 0x00;
        }
    }
    return turnstiles;
}

list_t* InitTokens(const char *filename)
{
    list_t *tokens;

    tokens = ListCreateFromTextfile(filename, 1);
    if (tokens == NULL) {
        printf("Error loading %s file.\n", filename);
        exit(0);
    }
    return tokens;
}

list_t* InitHeader(const char *filename)
{
    list_t *header;

    header = ListCreateFromTextfile(filename, 1);
    if (header == NULL) {
        printf("Error loading %s file.\n", filename);
        exit(0);
    }
    return header;
}

list_t* AppendTokens(list_t *header, list_t *tokens)
{
    list_t *headers, *newheader;
    list_node_t *newnode;

    //Create list of headers, add default header copyies in ammount of
    //available tokens. To each copy of header append unique token from tokens list

    headers = ListCreate();
    if (headers == NULL) {
        printf("Error allocating memory for headers while appending tokens.\n");
        exit(0);
    }
    for (int i = 0; i < tokens->length; i++) {
        newheader = ListCreate();
        for (int j = 0; j < header->length; j++) {
            ListAdd(newheader, ListNodeCopy(ListIndex(header, j)));
        }
        ListAdd(newheader, ListNodeCopy(ListIndex(tokens, i)));
        ListAdd(newheader, ListNodeCreateFromChar("\r\n",2));
        ListAdd(newheader, ListNodeCreateFromChar("\r\n",2));
        newnode = ListNodeCreate();
        newnode->data_size = 0;
        newnode->data = newheader;
        ListAdd(headers, newnode);
    }
    return headers;
}

int main()
{
    list_t *server,
            *turnstiles,
            *tokens,
            *check_header,
            *use_header,
            *check_headers,
            *use_headers;
    char *reply;
    int result;
    uint32_t card_id;
    zkt_connection_t connection;
    TicketStatusStruct *ticket_status;

    server = InitServer("server.conf");
    turnstiles = InitTurnstiles("turnstiles.conf");
    tokens = InitTokens("tokens.conf");
    check_header = InitHeader("check_header.conf");
    check_headers = AppendTokens(check_header, tokens);
    use_header = InitHeader("use_header.conf");
    use_headers = AppendTokens(use_header, tokens);


    if (tokens->length < turnstiles->length) {
        printf("Not enogh tokens (%u) for all turnstiles (%u).\n",
               tokens->length, turnstiles->length);
        exit(0);
    }

    connection.dev_type = "C3-200";
    connection.ip = ListIndexData(turnstiles, 0);
    connection.port = 4370;
    connection.ssid = 0x0000;
    connection.reply_counter = 0xFFF0;
    connection.status = ZKT_STATUS_DISCONNECTED;

    zkt_connect(&connection);
    while (1){
        if (connection.status == ZKT_STATUS_CONNECTED) {
            result = zkt_get_events(&connection, &reply);
            if (result > 0) {
                card_id = zkt_get_wiegand34_id(reply, result);
                if (card_id != 0) {
                    printf("Received ID# %u (dec) %08X (hex)\n", card_id, card_id);
                    if (card_id == 1805087518) break;
                    ticket_status = CheckTicket(card_id, ListIndexData(check_headers, 0), ListIndexData(server, 0), atoi(ListIndexData(server, 1)));
                    if (ticket_status != NULL) {
                        printf("Code = %d.\n", ticket_status->code);
                        printf("Lamp = %d.\n", ticket_status->lamp);
                        if (ticket_status->lamp == 1)
                            zkt_open_door(&connection, ZKT_OUTPUT_AUX, 1, 3);
                        if (ticket_status->code > 0)
                            zkt_open_door(&connection, ZKT_OUTPUT_DOOR, 1, 1);
                        free(ticket_status);
                    }
                }
                free(reply);
            }
        } else {
            zkt_connect(&connection);
        }
        usleep(500000);
    }
    zkt_disconnect(&connection);

    return 0;
}
