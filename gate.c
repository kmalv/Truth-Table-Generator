#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gate.h"

#define NUM_KIND 9

typedef struct key_val_t
{
    char* key;
    int value;
} key_val_t;

// Lookup dictionary for string representations of the gates and their int values
static key_val_t kind_dict[] = { { "AND", AND },
                                { "OR", OR }, 
                                { "NAND", NAND },
                                { "NOR", NOR },
                                { "XOR", XOR },
                                { "NOT", NOT },
                                { "PASS", PASS },
                                { "DECODER", DECODER },
                                { "MULTIPLEXER", MULTIPLEXER } };

// Converts a string to the respective gate kind
int match_to_kind(char *str)
{
    for (int i = 0; i < NUM_KIND; i++)
    {
        if (strcmp(kind_dict[i].key, str) == 0)
            return kind_dict[i].value;
    }
    return -1;
}

// Convert a decimal number to a decimal value
int* dec_to_binary(int num, int digits)
{
    int *binary = malloc(digits * sizeof(int));
    int index = digits - 1;

    while (index >= 0)
    {
        binary[index--] = num & 1;
        num >>= 1;
    }

    return binary;
}

// Calculates 2^exponent as an integer
int power(int exponent)
{
    return 1 << exponent;
}

// Gets input size of a gate
kind_t get_input_size(gate_info_t *gate)
{
    if (gate == NULL)
        return -1;

    int kind = gate->kind;
    int size = gate->size;

    return get_input_size_kind(kind, size);
}

// Gets output size of a gate
kind_t get_output_size(gate_info_t *gate)
{
    if (gate == NULL)
        return -1;
        
    int kind = gate->kind;
    int size = gate->size;

    return get_output_size_kind(kind, size);
}

// Get input size given kind_t and size
kind_t get_input_size_kind(kind_t kind, int size)
{
    if (kind == PASS || kind == NOT)
        return 1;
    if (kind == MULTIPLEXER)
        return (power(size) + size);
    if (kind == DECODER)
        return size;
    else
        return 2;
}

// Get output size given kind_t and size
kind_t get_output_size_kind(kind_t kind, int size)
{
    if (kind == PASS || kind == NOT)
        return 1;
    if (kind == MULTIPLEXER)
        return 1;
    if (kind == DECODER)
        return power(size);
    else
        return 1;    
}

// Print a gate's kind, input names, and output name
void print_gate_list(gate_info_t *head)
{
    gate_info_t *ptr = head;
    while (ptr != NULL)
    {
        printf("GATE: %i\n", ptr->kind);
        int num_inputs = get_input_size(ptr);
        int num_outputs = get_output_size(ptr);
        
        // Inputs
        for (int i = 0; i < num_inputs; i++)
        {
            printf("    IN: %i\n", ptr->inputs[i]);
        }
        // Outputs
        for (int i = 0; i < num_outputs; i++)
        {
            printf("    OUT: %i\n", ptr->outputs[i]);
        }

        ptr = ptr->next;
    }
}

// Free a linked list of gates
void free_gate_list(gate_info_t *head)
{
    gate_info_t *ptr = head;

    while (head != NULL)
    {
        ptr = head;

        // Free inputs/outs
        free(ptr->inputs);
        free(ptr->outputs);

        head = ptr->next;
        free(ptr);
    }
}

// Inserts a gate into a linked list of gates
void insert_gate_into_list(gate_info_t **head, gate_info_t *gate)
{
    if (*head == NULL)
    {
        *head = gate;
        return;
    }

    gate_info_t *ptr = *head;
    gate_info_t *prev = NULL;

    while (ptr != NULL)
    {
        prev = ptr;
        ptr = ptr->next;
    }

    prev->next = gate;
}

// Creates an array of gates given a linked list of gates
gate_info_t** create_gate_info_arr(gate_info_t *head, int num_gates)
{
    gate_info_t **gates = malloc(num_gates * sizeof(gate_info_t *));

    int i = 0;
    gate_info_t *ptr = head;
    while (ptr != NULL)
    {
        gates[i] = ptr;
        i++;
        ptr = ptr->next;
    }

    return gates;
}

// Creates a gate_node_t with a null next pointer
gate_info_t* create_gate_node(kind_t type, int size, int *inputs, int *outputs)
{
    gate_info_t *gate_node = malloc(sizeof(gate_info_t));
    gate_node->kind     = type;
    gate_node->size     = size;
    gate_node->inputs   = inputs;
    gate_node->outputs  = outputs;
    gate_node->next     = NULL;

    return gate_node;
}

// AND gate
void and_gate(int **table, int rows, int output, int input1, int input2)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = table[i][input1] && table[i][input2];
    }
}

// OR gate
void or_gate(int **table, int rows, int output, int input1, int input2)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = table[i][input1] || table[i][input2];
    }
}

// NAND gate
void nand_gate(int **table, int rows, int output, int input1, int input2)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = !(table[i][input1] && table[i][input2]);
    }
}

// NOR gate
void nor_gate(int **table, int rows, int output, int input1, int input2)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = !(table[i][input1] || table[i][input2]);
    }
}

// XOR gate
void xor_gate(int **table, int rows, int output, int input1, int input2)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = table[i][input1] ^ table[i][input2];
    }
}

// PASS gate
void pass_gate(int **table, int rows, int output, int input1)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = table[i][input1];
    }
}

// NOT gate
void not_gate(int **table, int rows, int output, int input1)
{
    for (int i = 0; i < rows; i++)
    {
        table[i][output] = !table[i][input1];
    }
}

// DECODER gate
void decoder_gate(int **table, int rows, int size, int *inputs, int *outputs)
{
    for (int i = 0; i < rows; i++)
    {
        int x = 0;
        for (int j = 0; j < power(size); j++) 
        {
            table[i][outputs[j]] = 0;
        }
        for (int j = 0; j < size; j++) 
        {
            x += table[i][inputs[j]] * power(size - j - 1);
        }
        table[i][outputs[x]] = 1;
    }
}

// MULTIPLEXER gate
void multiplexer_gate(int **table, int rows, int size, int output, int *inputs, int *selectors)
{
    for (int i = 0; i < rows; i++)
    {
        int value = 0;
        for (int j = 0; j < size; j++)
        {
            // printf("sel %i %i\n", j, selectors[j]);
            value += table[i][selectors[j]] * power(size - j - 1);
        }
        // printf("val %i\n", value);
        table[i][output] = table[i][inputs[value]];
    }
}