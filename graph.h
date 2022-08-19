#ifndef GRAPH_H
#define GRAPH_H

typedef struct adj_list_node_t
{
    int dest;
    struct adj_list_node_t *next;
} adj_list_node_t;

typedef struct graph_t
{
    int vertices;
    struct adj_list_node_t **adj_list;
} graph_t;

int* top_sort(graph_t *graph);
void add_edge(graph_t **graph, int src, int dest);
graph_t* new_graph(int vertices);
adj_list_node_t* new_adj_list_node(int dest);
void free_graph(graph_t *graph);

#endif