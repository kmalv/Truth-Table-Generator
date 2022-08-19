#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "str.h"
#include "gate.h"
#include "graph.h"

// enum representing input or output
typedef enum io_t 
{
    INPUT, OUTPUT
} io_t;

typedef enum indices_t
{
    INPUTS_SIZE, OUTPUTS_SIZE, VARS_SIZE, GATES_SIZE
} indices_t;

int io_reader(FILE *, io_t, int *, str_node_t **);
char** read_str_to_arr(FILE *, int);
int invalid_file(int, char **);
int* gate_reader(FILE *, int, int *, str_node_t **);
graph_t* create_gate_graph(gate_info_t **, int);
void create_table(str_node_t **, gate_info_t **, int *);
int** truthtable(int, int);
void init_inputs(int **table, int rows, int num_inputs);
void fill_ones_and_zeros(int **, str_node_t **, int, int, int);
void fill_table(int **table, gate_info_t *curr, int rows);
void free_table(int **table, int rows, int num_params);
void print_truthtable(int **, int, int, int);

int main(int argc, char** argv)
{
    FILE *file;
    
    // Check for invalid arguments -- we read from stdin otherwise
    if (invalid_file(argc, argv))
        file = stdin;
    // If that worked we can read the file
    else
        file = fopen(argv[1], "r");
    
    int num_inputs, num_outputs, num_vars = 0;
    str_node_t *vars = NULL;

    // Read number/names of inputs and outputs
    num_inputs = io_reader(file, INPUT, &num_vars, &vars);
    num_outputs = io_reader(file, OUTPUT, &num_vars, &vars);

    // Linked list of gates
    int num_gates = 0;
    gate_info_t *gates = NULL;

    // Read gates
    char buffer[17];
    while (fscanf(file, " %16s", buffer) != EOF)
    {
        int kind = match_to_kind(buffer);
        // printf("kind %i\n", kind);
        
        // If kind is valid
        if (kind != -1)
        {
            // Standard input, output, and size (AND, OR, XOR, etc)
            int in_len = 2, out_len = 1, size = 2;

            if (kind == PASS || kind == NOT)
            {
                in_len = 1; out_len = 1; size = 1;
            }
            else if (kind == MULTIPLEXER)
            {
                if (fscanf(file, "%d", &size) == EOF)
                    break;
                in_len = get_input_size_kind(kind, size);
                out_len = get_output_size_kind(kind, size);
            }
            else if (kind == DECODER)
            {
                if (fscanf(file, "%d", &size) == EOF)
                    break;
                in_len = get_input_size_kind(kind, size);
                out_len = get_output_size_kind(kind, size);
            }

            // print_str_list(vars);
            
            // Read inputs
            int* inputs = gate_reader(file, in_len, &num_vars, &vars);
            int* outputs = gate_reader(file, out_len, &num_vars, &vars);

/*             for (int i = 0; i < in_len; i++)
            {
                printf("in %i %i\n", i, inputs[i]);
            }
            for (int i = 0; i < out_len; i++)
            {
                printf("out %i %i\n", i, outputs[i]);
            } */

            insert_gate_into_list(&gates, create_gate_node(kind, size, inputs, outputs));
            num_gates++;
        }
        else
            break;
    }
    
    // Now do the math!!!
    // First, create an array of gates and vars
    gate_info_t **gates_arr = create_gate_info_arr(gates, num_gates);
    str_node_t **vars_arr = create_str_node_arr(num_vars, vars);
    
    // print_str_list(vars);
    // print_str_arr(vars_arr, num_vars);
    // print_gate_list(gates);

    int sizes[4] = { num_inputs, num_outputs, num_vars, num_gates };
    create_table(vars_arr, gates_arr, sizes);

    // Freeing
    free_gate_list(gates);
    free_str_list(vars);
    free(gates_arr);
    free(vars_arr);

    return EXIT_SUCCESS;
}

// Creates the final truth table :)
void create_table(str_node_t **vars_arr, gate_info_t **gates_arr, int *sizes)
{
    // Make graph of gates and sort it
    graph_t *graph = create_gate_graph(gates_arr, sizes[GATES_SIZE]);
    int *sorted = top_sort(graph);
    
/*     for (int i = 0; i < sizes[GATES_SIZE]; i++)
    {
        printf("%i ", sorted[i]);
    }
    printf("\n"); */

    int num_inputs = sizes[INPUTS_SIZE];
    int num_outputs = sizes[OUTPUTS_SIZE];
    //nt num_gates = sizes[GATES_SIZE];
    int num_vars = sizes[VARS_SIZE];
    int rows = power(sizes[INPUTS_SIZE]);

    // Now for the fun stuff, solve the logic gate!!!
    int **table = truthtable(num_vars, rows);

    // Set up
    init_inputs(table, rows, num_inputs);
    fill_ones_and_zeros(table, vars_arr, rows, num_inputs, num_vars);
    // Fill up inputs
    for (int i = 0; i < sizes[GATES_SIZE]; i++)
    {
        gate_info_t *curr = gates_arr[sorted[i]];
/*         for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < num_vars; j++)
            {
                printf("%i ", table[i][j]);
            }
            printf("\n");
        } */
        fill_table(table, curr, rows);
    }

    print_truthtable(table, rows, num_inputs, num_outputs);
    free_graph(graph);
    free(sorted);
    free_table(table, rows, sizes[VARS_SIZE]);
}

void fill_table(int **table, gate_info_t *curr, int rows)
{
    int kind = curr->kind;
    if (kind == PASS || kind == NOT)
    {
        int input1 = curr->inputs[0];
        int output = curr->outputs[0];

        if (kind == PASS)
            pass_gate(table, rows, output, input1);
        else
            not_gate(table, rows, output, input1);
    }
    else if (kind == DECODER)
    {
        int size = curr->size;
        int *inputs = curr->inputs;
        int *outputs = curr->outputs;
        decoder_gate(table, rows, size, inputs, outputs);
    }
    else if (kind == MULTIPLEXER)
    {
        int size = curr->size;
        int output = curr->outputs[0];
        int start = power(size);
        int total = get_input_size_kind(MULTIPLEXER, size);

        // Set up inputs and selectors
        int *inputs = malloc(power(size) * sizeof(int));
        for (int i = 0; i < power(size); i++)
            inputs[i] = curr->inputs[i];
        
        int *selectors = malloc(size * sizeof(int));
        for (int i = start, j = 0; i < total; i++, j++)
        {
            selectors[j] = curr->inputs[i];
        }

        // printf("%p\n", selectors);
        multiplexer_gate(table, rows, curr->size, output, inputs, selectors);
        free(inputs);
        free(selectors);
    }
    else
    {
        int input1 = curr->inputs[0];
        int input2 = curr->inputs[1];
        int output = curr->outputs[0];

        // printf("i1 %i i2 %i o %i\n", input1, input2, output);
        switch (kind)
        {
            case AND:
            {
                and_gate(table, rows, output, input1, input2);
                break;
            }
            case OR:
            {
                or_gate(table, rows, output, input1, input2);
                break;
            }
            case NAND:
            {
                nand_gate(table, rows, output, input1, input2);
                break;
            }
            case NOR:
            {
                nor_gate(table, rows, output, input1, input2);
                break;
            }
            case XOR:
            {
                xor_gate(table, rows, output, input1, input2);
                break;
            }
        }
    }
}

// Initialize input truth values
void init_inputs(int **table, int rows, int num_inputs)
{
    for (int i = 0; i < rows; i++)
    {
        int *binary = dec_to_binary(i, num_inputs);
        for (int j = 0; j < num_inputs; j++)
        {
            table[i][j] = binary[j];
        }
        free(binary);
    }
}

// Frees the truth table
void free_table(int **table, int rows, int num_params)
{
    for (int i = 0; i < rows; i++)
    {
        free(table[i]);
    }
    free(table);
}

// Prints out the truth table
void print_truthtable(int **table, int rows, int num_inputs, int num_outputs)
{
    int total = num_inputs + num_outputs;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < num_inputs; j++)
        {
            printf("%i ", table[i][j]);
        }
        printf("|");
        for (int j = num_inputs; j < total; j++)
        {
            printf(" %i", table[i][j]);
        }
        printf("\n");
    }
}

// Create initial truth table with only input values
int** truthtable(int num_vars, int rows)
{   
    int** table = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        // Reserve enough space for all values
        table[i] = malloc(num_vars * sizeof(int));
    }

    return table;
}

// Fills the columns w/ 1 and 0 with 1's and 0's :)
void fill_ones_and_zeros(int **table, str_node_t **vars_arr, int rows, int num_inputs, int num_vars)
{
    for (int i = 0; i < num_vars; i++)
    {
        // Constant 1 - all true
        if (strcmp(vars_arr[i]->value, "1") == 0)
        {
            int col = vars_arr[i]->index;
            for (int i = 0; i < rows; i++)
            {
                table[i][col] = 1;
            }
        }
        // Constant 0 - all false
        if (strcmp(vars_arr[i]->value, "0") == 0)
        {
            int col = vars_arr[i]->index;
            for (int i = 0; i < rows; i++)
            {
                table[i][col] = 0;
            }
        }
    }
}

// Creates a graph and fills in edges, given an array of gates
graph_t* create_gate_graph(gate_info_t **gates_arr, int num_gates)
{
    graph_t *graph = new_graph(num_gates);
    
    // Go through gates and find edges
    for (int i = 0; i < num_gates; i++)
    {
        // printf("num gates %i\n", num_gates);
        int out_size = get_output_size(gates_arr[i]);
        // Go through each output in the list
        for (int j = 0; j < out_size; j++)
        {
            int var_index = gates_arr[i]->outputs[j];
            // Go through the rest of the list, seeing if
            // any gates require gates_arr[i]'s output as
            // an input
            for (int k = 0; k < num_gates; k++)
            {
                // printf("k is %i i is %i\n", k, i);
                int curr_in_size = get_input_size(gates_arr[k]);
                // printf("%i \n", curr_in_size);
                for (int l = 0; l < curr_in_size; l++)
                {
                    int curr_index = gates_arr[k]->inputs[l]; 
                    if (var_index == curr_index)
                    {
                        add_edge(&graph, i, k);
                    }
                }
            }
        }
    }

    return graph;
}

// Reads in variables of io_t, returns numbers of variables read
int io_reader(FILE *file, io_t type, int *len, str_node_t **head)
{
    // Read in the number of inputs/outputs
    int io_size;
    char *format;
    if (type == INPUT)
        format = "INPUT %d";
    else
        format = " OUTPUT %d";
    
    if (fscanf(file, format, &io_size) == EOF)
        return -1;
    
    char buffer[17];
    for (int i = 0; i < io_size; i++)
    {
        fscanf(file, "%16s", buffer);
        insert_str_list(head, create_str_node(buffer, (*len)++));
    }
    
    return io_size;
}

// Reads in a gate, adds unseen variables to vars linked list
int* gate_reader(FILE *file, int n, int *num_vars, str_node_t **vars)
{
    char buffer[17];
    int *indices = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        // printf("%i\n", n);
        if (fscanf(file, "%16s", buffer) == EOF)
            return NULL;
        // printf("BUFF %s\n", buffer);
        // Look inside vars for the index of the variable
        str_node_t *ptr = *vars;
        int seen = 0;
        while (ptr != NULL)
        {
            if (strcmp(ptr->value, buffer) == 0)
            {
                // printf("Word: %s Index: %i\n", ptr->value, ptr->index);
                // printf("    EQUAL\n");
                seen = 1; 
                break;
            }
            else
                ptr = ptr->next;
        }

        // Get index of the variable
        if (seen)
        {
            // printf("entering %i\n", ptr->index);
            indices[i] = ptr->index; 
        }
        else
        {
//            printf("num vars rn: %i\n", *num_vars);
            indices[i] = *num_vars;
            insert_str_list(vars, create_str_node(buffer, (*num_vars)++));
/*             for (int i = 0; i < n; i++)
            {
                printf("indices %i: %i\n", i, indices[i]);
            } */
        }
    }

    return indices;
}

// Checks for invalid arguments
int invalid_file(int argc, char** argv)
{
    // Only 1 argument (./truthtable)
    if (argc < 2)
        return 1;
    // Empty arguments
    if (argv == NULL)
        return 1;
    // Empty arguments
    if (argc > 1 && argv[1] == NULL)
        return 1;
    // Too many arguments
    if (argc > 2)
        return 1;
    // Now to check that the value is valid:
    FILE *file = fopen(argv[1], "r");

    // File could not be opened
    if (file == NULL)
        return 1;
    return 0;
}