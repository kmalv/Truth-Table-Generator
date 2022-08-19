#ifndef STR_H
#define STR_H

typedef struct str_node_t
{
    int index;
    char *value;
    struct str_node_t *next;
} str_node_t;

char* new_str(char *src);
void free_str_arr(char **arr, int len);
void print_str_list(str_node_t *head);
void free_str_list(str_node_t *head);
void insert_str_list(str_node_t **head, str_node_t *to_add);
str_node_t* create_str_node(char *value, int index);
str_node_t** create_str_node_arr(int len, str_node_t *head);
void print_str_arr(str_node_t **arr, int size);

#endif