#include <stdlib.h>
#include <stdio.h>

#include "graph.h"

int dequeue(int, int);
int enqueue(int, int);
void free_adj_list(adj_list_node_t *);

void print_array(int len, int *arr)
{
    for (int i = 0; i < len; i++)
    {
        printf("%i ", arr[i]);
    }
    printf("\n");
}

// Create a new adj list node
adj_list_node_t* new_adj_list_node(int dest)
{
    adj_list_node_t *node = malloc(sizeof(adj_list_node_t));
    node->dest = dest;
    node->next = NULL;

    return node;
}

// Initialize a graph with given vertices 
graph_t* new_graph(int vertices)
{
    graph_t *graph = malloc(sizeof(graph_t));
    graph->vertices = vertices;
    graph->adj_list = malloc(vertices * sizeof(adj_list_node_t *));

    // Set every adj_list to null
    for (int i = 0; i < vertices; i++)
    {
        graph->adj_list[i] = NULL;
    }

    return graph;
}

// Add an edge to the graph between the source vertex and dest vertex
void add_edge(graph_t **graph, int src, int dest)
{
    adj_list_node_t **graph_adj_list = (*graph)->adj_list;
    adj_list_node_t *node = new_adj_list_node(dest);
    // This new node points to the source node's adjacency list
    node->next = graph_adj_list[dest];
    // Add this node to the adjacency list
    graph_adj_list[src] = node;
}

// Topological sort using Kahn's algorithm
int* top_sort(graph_t *graph)
{
    int vertices = graph->vertices;
    adj_list_node_t **adj_list = graph->adj_list;
    
    // Store number of indegrees for each vertice
    // (number of edges coming towards the indexed vertex)
    int *indegrees = calloc(vertices, sizeof(int));

    for (int i = 0; i < vertices; i++)
    {
        adj_list_node_t *ptr = adj_list[i];
        while (ptr != NULL)
        {
            indegrees[ptr->dest]++;
            ptr = ptr->next;
        }
    }

    int *queue = malloc(vertices * sizeof(int));
    int queue_size = 0, queue_front = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (indegrees[i] == 0)
        {
            queue[queue_size] = i;
            queue_size = enqueue(queue_size, vertices);
        }
    }

    // printf("INDEGREES:\n");
    // print_array(vertices, indegrees);

    int visited = 0;
    int *sorted = malloc(vertices * sizeof(int));
    int sorted_i = 0;

    // printf("v: %i ver %i\n", visited, vertices);
    while (visited != vertices) 
    {
        // printf("QUEUE:\n");
        // print_array(vertices, queue);
        int dequeued = queue[queue_front];
        sorted[sorted_i] = dequeued;
        queue_front = dequeue(queue_front, vertices);
        sorted_i++;

        adj_list_node_t *ptr = adj_list[dequeued];
        while (ptr != NULL)
        {
            indegrees[ptr->dest]--;
            if (indegrees[ptr->dest] == 0)
            {
                queue[queue_size] = ptr->dest;
                queue_size = enqueue(queue_size, vertices);
            }
            ptr = ptr->next;
        }

        visited++;
    }

    //print_array(vertices, sorted);
    free(indegrees);
    free(queue);

    return sorted;
}

// Returns new front when dequeueing
int dequeue(int queue_front, int queue_max)
{
    if (queue_front + 1 == queue_max)
        return 0;
    queue_front++;
    return queue_front;
}

// Returns new size after enqueueing
int enqueue(int queue_size, int queue_max)
{
    if (queue_size + 1 == queue_max)
        return 0;
    queue_size++;
    return queue_size;
}

// Frees memory associated with graph
void free_graph(graph_t *graph)
{
    int v = graph->vertices;
    for (int i = 0; i < v; i++)
    {
        adj_list_node_t *head = graph->adj_list[i];

        while (head != NULL)
        {
            adj_list_nodptr = head;
            head = ptr->next;
            free(ptr);
        }
    }
    free(graph->adj_list);
    free(graph);
}

// Frees an adjacency list
void free_adj_list(adj_list_node_t *head)
{
    adj_list_node_t *ptr = head;

    while (head != NULL)
    {
        ptr = head;
        head = head->next;
        free(ptr);
    }
}