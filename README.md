# Mini-Shell
This project essentially creates a mini shell in the C compiler itself. It takes in a set of commands and files. The commands are used for compiling the code with different 
inputs, and the files are used to test these compilations in different cases. The output returns if it is successful or if it failed. 
There are three main functions for this code:

# read_spsss_commands: 
Something that takes the input string of commands and test and stores the individual parts in a struct. This struct stores a pointer for each of the two 
lists.

# compile_program: 
The code is forked to concurrently run the different compilations at the same time. I all of these compilations can be run without any problems, we return 0 for success and 1 
for fail. 

# test_program:
This code looks at the tests stored in struct from the previous function and runs the tests on each file. Even if a test fails, the code runs to test all of the remaining commands, since they could still run properly. The function returns the number of tests that were successful after running.

There is also a function to clear any memory not being used.
