// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list_node_t{
    struct list_node_t      *previous;
    struct list_node_t      *next;
	unsigned int            data_size;
    void                    *data;
} list_node_t;

typedef struct list_t{
    list_node_t             *first;
    list_node_t             *last;
    list_node_t             **index;
    char                    modified;
    unsigned int            length;
} list_t;

extern list_t* ListCreate(void);
extern list_t* ListCreateFromTextfile(const char *filename, int CRLF);
extern list_node_t* ListNodeCreate();
extern list_node_t* ListNodeNew(void *data, size_t datasize);
extern list_node_t* ListNodeCopy(list_node_t* node);
extern list_node_t* ListNodeCreateFromChar(const char *data, size_t datasize);
extern list_node_t* ListNodeCreateFromString(const char *text);
extern int ListAdd(list_t *list, list_node_t *node);
extern int ListDelete(list_t *list, list_node_t *node);
extern int ListRebuildIndex(list_t *list);
extern list_node_t* ListIndex(list_t *list, unsigned int index);
extern void* ListIndexData(list_t *list, unsigned int index);
extern unsigned int ListIndexDataSize(list_t *list, unsigned int index);
extern int ListInsert(list_t *list, list_node_t *node, unsigned int position);
extern int ListDestroy(list_t **list);
extern int ListToChar(list_t *list, char **destination);
extern int ListToString(list_t *list, char **destination);

#endif // LIST_H
