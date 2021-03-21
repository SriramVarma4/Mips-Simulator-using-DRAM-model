# COL216 minor

## Aim

The aim of this assignment is to create a mips simulator using C++ with given rules

## Contents of my folder:
- Code file(test3.cpp)
- test cases

## Discussion

- MIPS is known as a Load/Store register File machine. This means most of the execution statements will occur over the smaller and faster registers, while the slow but large memory will store the data.
- In our implementation of the MIPS, we store 4 byte for each of the tokens, which translates to the 4 bytes in memory arrray. Thus each 4 contiguous set of bytes in the memory array represents a code token.
- After all tokens in from the code file are parsed into the memory array, the rest of the space is left for storing other variables. These too are formatted to take 4 bytes at once and can be accessed from the file via register commands.
- Due to implentation methodology, each seperate token can only be 4 characters long, thus the usual $zero register had to be condensed to $z, limit on integers is from (9999-~999) and no direct support for offset's in lw and sw commands.
- created row buffer checked cases accordingly and checked if we need to do copy new row buffer or can use current row buffer. 

## Input

The input will be a text file containing code for MIPS Assembly language and delay counters
This input file will be taken in command line after executable.
## Method

We defined memory address and register file using byte 2d array of 8 bits and a 1d array 32 bits sizes respectively.

- we will convert the given text file into a string
- Divide the big string into 4 byte components, each of which will be stored in the memory and can be executed upon.
- start iterating over the memory address and execute the corresponding keyword

## Output
 Prints the portion of the memory array used to encode the code file, then on each cycle, the program will print the  contents of registers and the address of execution in the memory array.It will also print the number of clock cycles

## Testing
I tested my program with the attached 4 testcases. These all showed expected results.

## Setup
 for compiling use 

     g++ test3.cpp
 
 for run use

     ./test3 <path/to/file> <row delay> <col delay>

