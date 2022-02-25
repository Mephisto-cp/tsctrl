// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "ticketapi.h"

#define TICKETAPI_DEBUG_MSG

extern TicketStatusStruct* CheckTicket(uint32_t ticket_num, list_t *header, char *ip, uint16_t port)
{
    int         result,
                socket_desc;
    char        ticket_num_char[11],
                *request,
                *reply,
                *reply_body,
                *search_position,
                *end_ptr;
    int         request_length = 0,
                reply_length = 0;
    list_t      *reply_list;
    list_node_t *reply_node;
    TicketStatusStruct *ticket_status;

    //Add request body containing ticket id
    sprintf(ticket_num_char, "%u", ticket_num);
    ListAdd(header, ListNodeCreateFromChar("{\"uuid\":\"",strlen("{\"uuid\":\"")));
    ListAdd(header, ListNodeCreateFromChar(ticket_num_char,strlen(ticket_num_char)));
    ListAdd(header, ListNodeCreateFromChar("\"}",strlen("\"}")));
    request_length = ListToChar(header, &request);
    result = tcp_connect(&socket_desc, ip, port);
    if (result < 0) {
        printf("Unable to connect to %s port %u.\n", ip, port);
        return NULL;
    }
    tcp_send(request, request_length, socket_desc);
    reply_list = ListCreate();
    while (1) {
        reply_length = tcp_recv(&reply, socket_desc);
        if (reply_length > 0) {
            reply_node = ListNodeCreateFromChar(reply, reply_length);
            ListAdd(reply_list, reply_node);
        } else {
            if (reply_length < 0) printf("Error recieving reply.\n");
            break;
        }
    }

    ticket_status = malloc(sizeof(TicketStatusStruct));
    if (ticket_status == NULL) {
        printf("Error allocating mamory for ticket_status structure.\n");
        exit(0);
    }
    ticket_status->code = 0;
    ticket_status->lamp = 0;
    ListToChar(reply_list, &reply);
//    printf("Reply:\n");
//    for (int i=0; i<reply_length; i++) {
//        putc(reply[i], stdout);
//    }
//    printf("\n");
    reply_body = strstr(reply, "\r\n\r\n") + 4;
    search_position = strstr(reply_body, "\"code\":");
    if (search_position != NULL) {
        search_position += strlen("\"code\":");
        ticket_status->code = strtol(search_position, &end_ptr, 10);
    }
    search_position = strstr(reply_body, "\"lamp\":");
    if (search_position != NULL) {
        search_position += strlen("\"lamp\":");
        ticket_status->lamp = strtol(search_position, &end_ptr, 10);
    }
    //Remove request body containing ticket id
    for (int i=0; i<3; i++) {
        ListDelete(header, header->last);
    }
    free(reply);
    ListDestroy(&reply_list);
    return ticket_status;
}

