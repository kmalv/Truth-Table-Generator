# Truth Table Generator
## by Kayla Alvarado for Rutgers University Fall 2021 CS211

Given a text file containing a list of simple logic gates, their inputs, and their outputs, this program produces the resulting truth table of the final circuit.

Gates can be listed in any order. Kahn's algorithm has been used to sort gates and evaluate them appropriately. For example, gate A's inputs may be gate B's output. Gate B will be evaluated before Gate A is. 

Test cases are included in the `data` folder.

To run this program, use `make`, and then enter the following commands into the command line:

```bash
./truthtable data/example.txt
```

where `example` is the name of any of the files beginning with `test` in the `data` folder.

For any questions feel free to email me at kayla.alvarado@rutgers.edu! 