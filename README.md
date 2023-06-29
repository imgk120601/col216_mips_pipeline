# MIPS Processor

This code is the starter code for Assignment 2 of the 2023 spring offering of the Computer Architecture course (2202-COL216).

## Files
- `MIPS_Processor.hpp` contains the implementation of the instruction parser and a non-pipelined implementation of the MIPS processor.
- `sample.cpp` contains a sample usage of the `MIPS_Architecture` struct.
- `sample.asm` contains a sample assembly program that can be run on the processor.

## Usage
After cloning the repository, run the following commands to compile and run the sample program:
```bash
$ make all
$ ./sample sample.asm
```

//---EDIT BY GAURAV KUMAR -2019CS50430
HOW TO RUN THE CODE

$make all
$ ./5stage sample1.asm  (to run 5stage and our input is in sample1.asm)
$ ./5stage_bypass sample1.asm  (to run 5stage_bypass and our input is in sample1.asm)
$ ./79stage sample1.asm  (to run 79stage and our input is in sample1.asm)
$ ./79stage_bypass sample1.asm  (to run 79stage_bypass and our input is in sample1.asm)
