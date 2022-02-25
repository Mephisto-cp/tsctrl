// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "list.h"

extern list_t* ListCreate(void){

    list_t *list;

    list = malloc(sizeof(list_t));
    if (list != NULL) {
        list->first = 0;
        list->last = 0;
        list->length = 0;
        list->modified = 0;
        list->index = 0;
    }
    return list;
}

extern list_t* ListCreateFromTextfile(const char *filename, int CRLF){

    FILE *textfile;
    list_t *list;
    list_node_t *node;
    char input_text[1024];
    char *result;

    list = ListCreate();
    if (list == NULL) return NULL;
    textfile = fopen(filename, "r");
    if (textfile == NULL) {
        ListDestroy(&list);
        return NULL;
    }
    while (!feof(textfile)) {
        result = fgets(input_text, sizeof(input_text), textfile);
        if (result != NULL) {
            if (CRLF != 0) {            //If you need CRLF instead of LF, dont't copy existing LF
                node = ListNodeCreateFromChar(input_text, strlen(input_text)-1);
            } else {
                node = ListNodeCreateFromChar(input_text, strlen(input_text));
            }
            if (node != NULL) {
                ListAdd(list, node);
            } else {
                ListDestroy(&list);
                break;
            }
            if (CRLF != 0) {            //If you need CRLF instead of LF, now add CRLF
                node = ListNodeCreateFromChar("\r\n", 2);
                if (node != NULL) {
                    ListAdd(list, node);
                } else {
                    ListDestroy(&list);
                    break;
                }
            }
        }
    }
    fclose(textfile);
    return list;
}

extern list_node_t* ListNodeCreate()
{
    list_node_t* node;

    node = malloc(sizeof(list_node_t));
    if (node != NULL) {
        node->data = 0;
        node->data_size = 0;
        node->next = 0;
        node->previous = 0;
    }
    return node;
}

extern list_node_t* ListNodeNew(void *data, size_t datasize)
{
    list_node_t *node;

    node = ListNodeCreate();
    if (node != NULL) {
        node->data = data;
        node->data_size = datasize;
    }

    return node;
}

extern list_node_t* ListNodeCopy(list_node_t* node)
{
    list_node_t* newnode;

    newnode = ListNodeCreate();
    if (newnode == NULL) return NULL;
    newnode->data = malloc(node->data_size);
    if (newnode->data == NULL) {
        free(newnode);
        return NULL;
    }
    memcpy(newnode->data, node->data, node->data_size);
    newnode->data_size = node->data_size;
    return newnode;
}

extern list_node_t* ListNodeCreateFromChar(const char *data, size_t data_size)
{
    list_node_t* node;

    node = ListNodeCreate();
    if (node != NULL) {
        node->data = malloc(data_size);
        if (node->data != NULL) {
            node->data_size = data_size;
            memcpy(node->data, data, data_size);
        } else {
            free(node);
            return NULL;
        }
    }
    return node;
}

extern list_node_t* ListNodeCreateFromString(const char *text)
{
    list_node_t* node;
	unsigned int text_length;

    node = ListNodeCreate();
    if (node != NULL) {
        text_length = strlen(text);
        node->data = malloc(text_length + 1);
        if (node->data != NULL) {
            node->data_size = text_length + 1;
            memcpy(node->data, text, node->data_size);
        } else {
            free(node);
            return NULL;
        }
    }
    return node;
}

int ListAdd(list_t *list, list_node_t *node)
{
    if (!(list && node)) {
        printf("Error: Unable to add node, pointer to list_t or list_node_t is NULL!\n");
        return 0;
	}
    if (list->length == 0) {
		list->first = node;
		list->last = node;
		node->previous = 0;
		node->next = 0;
	} else {
		node->next = 0;
		node->previous = list->last;
		list->last->next = node;
		list->last = node;
	}
    list->length++;
	list->modified = 1;
    return list->length;
}

int ListDelete(list_t *list, list_node_t *node)
{
    if (!(list && node)) {
        printf("Error: Pointer to list_t or list_node_t is NULL!\n");
		return 1;
	}
    if (node->previous != 0) {
		node->previous->next = node->next;
	} else {
		list->first = node->next;
	}
    if (node->next != 0) {
		node->next->previous = node->previous;
	} else {
		list->last = node->previous;
	}
	free(node->data);
	free(node);
    list->length--;
	list->modified = 1;
	return 0;
}

int ListRebuildIndex(list_t *list)
{
    list_node_t *current_node;
	unsigned int counter;

    if (!list) {
        printf("Error: could not rebuild index, list_t pointer is NULL!\n");
        return -1;
	}
    if (!list->length) {
		printf("Error: could not rebuild index, the list is empty!\n");
        return -1;
	}
	free(list->index);
    list->index = malloc(list->length * sizeof(list_node_t*));
    if (list->index == 0) return -1;
	counter = 0;
	current_node = list->first;
    while (current_node) {
		list->index[counter] = current_node;
		current_node = current_node->next;
		counter++;
	}
	list->modified = 0;
    return 0;
}

extern list_node_t* ListIndex(list_t *list,unsigned int index)
{
    if (list->modified)
        ListRebuildIndex(list);
	return list->index[index];
}

extern void* ListIndexData(list_t *list, unsigned int index)
{
    if (list->modified)
        ListRebuildIndex(list);
    return ListIndex(list, index)->data;
}

extern unsigned int ListIndexDataSize(list_t *list, unsigned int index)
{
    if (list->modified)
        ListRebuildIndex(list);
    return ListIndex(list, index)->data_size;
}

int ListInsert(list_t *list, list_node_t *node, unsigned int position)
{
    list_node_t *temp_node;

    if (!(list && node)) { //Check list and node pointer for being NULL
        printf("Error: Unable to insert node, pointer to list_t or list_node_t is NULL!\n");
		return 1;
	}
    if (list->length == 0) {
        printf("Error: Unable to insert node into empty list!\n");
        return 1;
    }
    if (position >= list->length) { //Check if position is in bounds of the list
		printf("Error: Unable to insert node, position is out of bounds!\n");
		return 1;
	}
    if (position == 0) { //If desired node position is the first position (index 0)
		node->next = list->first;
		list->first->previous = node;
		list->first = node;
        list->length++;
		list->modified = 1;
		return 0;
	} else { //If desired position is the any other then first
        temp_node = ListIndex(list, position);
		temp_node->previous->next = node;
		node->previous = temp_node->previous;
		node->next = temp_node;
		temp_node->previous = node;
        list->length++;
		list->modified = 1;
		return 0;
	}
}

int ListDestroy(list_t **list)
{
    list_node_t *current_node;
    list_node_t *temp_node;

    if (!list) {
        printf("Error: Unable to destroy list, pointer to list_t is NULL!\n");
		return 1;
	}
	current_node = (*list)->first;
    while (current_node) {
		free(current_node->data);
		current_node->data = 0;
		temp_node = current_node;
		current_node = current_node->next;
		free(temp_node);
		temp_node = 0;
	}
	free((*list)->index);
	free(*list);
	(*list) = 0;
	return 0;
}

int ListToChar(list_t *list, char **destination)
{
    unsigned int destination_size = 0;
    unsigned int bytes_copied = 0;
    list_node_t *node;

    if (list == NULL) { //Check list and node pointer for being NULL
        printf("Error: Unable to merge list, pointer to list_t is NULL!\n");
        return -1;
    }
    if (list->length == 0) {
        printf("Error: Unable to merge empty list!\n");
        return -1;
    }
    node = list->first;
    while (node) {
        destination_size += node->data_size;
        node = node->next;
    }
    if (destination_size > 0) {
        (*destination) = malloc(destination_size);
        if (*destination == NULL) {
            return -1;
        }
        node = list->first;
        while (node) {
            //memcpy((*destination)+bytes_copied, node->data, tmp);
            memcpy(&(*destination)[bytes_copied], node->data, node->data_size);
            bytes_copied += node->data_size;
            node = node->next;
        }
    } else {
        printf("Error: Unable to merge list, no data inside nodes!\n");
        return -1;
    }
    return bytes_copied;
}

int ListToString(list_t *list, char **destination)
{
	unsigned int destination_size = 0;
	unsigned int bytes_copied = 0;
	unsigned int tmp;
    list_node_t *node;

    if (!list) { //Check list and node pointer for being NULL
        printf("Error: Unable to merge list, pointer to list_t is NULL!\n");
		return 1;
	}
    if (list->length == 0) {
		printf("Error: Unable to merge empty list!\n");
		return 1;
	}
	node = list->first;
    while (node) {
		destination_size += strlen(node->data);
		node = node->next;
	}
	(*destination) = malloc(destination_size + 1);
	node = list->first;
    while (node) {
		tmp = strlen(node->data);
		//memcpy((*destination)+bytes_copied, node->data, tmp);
		memcpy(&(*destination)[bytes_copied], node->data, tmp);
		bytes_copied += tmp;
		node = node->next;
	}
	(*destination)[bytes_copied] = 0x00;
	return 0;
}
