#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

// Make a new string given a char array
char* new_str(char *src)
{
    char *dest = malloc((strlen(src) + 1) * sizeof(char));
    strcpy(dest, src);

    return dest;
}

// Prints index and value of linked list of string nodes
void print_str_list(str_node_t *head)
{
    str_node_t *ptr = head;
    while (ptr != NULL)
    {
        printf("index: %i, value: %s\n", ptr->index, ptr->value);
        ptr = ptr->next;
    }
}

void print_str_arr(str_node_t **arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("index: %i str: %s\n", arr[i]->index, arr[i]->value);
    }
}

// Frees a linked list of strings
void free_str_list(str_node_t *head)
{
    str_node_t *ptr = head;
    
    while (head != NULL)
    {
        ptr = head;
        free(ptr->value);
        head = ptr->next;
        free(ptr);
    }
}

// Inserts a node into a linked list of strings
void insert_str_list(str_node_t **head, str_node_t *to_add)
{
    if (*head == NULL)
    {
        *head = to_add;
        return;
    }

    str_node_t *ptr = *head;
    str_node_t *prev = NULL;
    while (ptr != NULL)
    {
        prev = ptr;
        ptr = ptr->next;
    }

    prev->next = to_add;
}

// Creates a string node
str_node_t* create_str_node(char *value, int index)
{
    str_node_t *node = malloc(sizeof(str_node_t));
    node->value = new_str(value);
    node->index = index;
    node->next  = NULL;

    return node;
}

// Creates an arrau of str nodes
str_node_t** create_str_node_arr(int len, str_node_t *head)
{
    str_node_t **strs = malloc(len * sizeof(str_node_t *));

    int i = 0;
    str_node_t *ptr = head;
    while (ptr != NULL)
    {
        strs[i] = ptr;
        i++;
        ptr = ptr->next;
    }

    return strs;
}

// Frees an array of strings
void free_str_arr(char **arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        free(arr[i]);
    }
    free(arr);
}
