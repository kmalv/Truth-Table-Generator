#ifndef GATE_H
#define GATE_H

// enum representing gate type
typedef enum kind_t 
{
    AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER
} kind_t;

// Represents a node of a gate
typedef struct gate_info_t
{
    kind_t kind;
    int size;
    int *inputs;
    int *outputs;
    struct gate_info_t *next;
} gate_info_t;

int* dec_to_binary(int num, int digits);
int power(int exponent);
int match_to_kind(char *str);
kind_t get_input_size(gate_info_t *gate);
kind_t get_output_size(gate_info_t *gate);
kind_t get_input_size_kind(kind_t kind, int size);
kind_t get_output_size_kind(kind_t kind, int size);
gate_info_t** create_gate_info_arr(gate_info_t *head, int num_gates);
gate_info_t* create_gate_node(kind_t type, int size, int *inputs, int *outputs);
void print_gate_list(gate_info_t *head);
void insert_gate_into_list(gate_info_t **head, gate_info_t *gate);
void free_gate_list(gate_info_t *head);

void and_gate(int **table, int rows, int output, int input1, int input2);
void or_gate(int **table, int rows, int output, int input1, int input2);
void nand_gate(int **table, int rows, int output, int input1, int input2);
void nor_gate(int **table, int rows, int output, int input1, int input2);
void xor_gate(int **table, int rows, int output, int input1, int input2);
void pass_gate(int **table, int rows, int output, int input1);
void not_gate(int **table, int rows, int output, int input1);
void decoder_gate(int **table, int rows, int size, int *inputs, int *outputs);
void multiplexer_gate(int **table, int rows, int size, int output, int *inputs, int *selectors);

#endif