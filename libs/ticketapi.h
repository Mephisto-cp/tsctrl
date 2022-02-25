// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef TICKETAPI_H
#define TICKETAPI_H

#include <inttypes.h>
#include <list.h>
#include "network.h"

typedef struct TicketStatusStuct {
    unsigned int code;
    unsigned int lamp;
} TicketStatusStruct;

extern TicketStatusStruct* CheckTicket(uint32_t ticket_num, list_t *header, char *ip, uint16_t port);


#endif // TICKETAPI_H
