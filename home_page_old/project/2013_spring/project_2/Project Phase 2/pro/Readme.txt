These are files provided for CSC3420 Project Phase2. "simulator_linux" is the reference simulator compiled by linux1 of our department. Make sure it has execution permission before running. One way to give execution permission to it is by example by: "chmod u+x simulator_linux"

For the assembler, make sure it has execution permission before running. One way to give execution permission to it is by example by
     "chmod u+x asm_linux"




A simple shell script "compile.sh" is provided for you to compile and run your simulator:

Usage:
 "./compile.sh machine_code.asc"

where machine_code.asc is the machine code in binary.



Files in this archive:
compile.sh 		- shell script to compile and run your simulator
components.h 		- header file defines the data structure representing each component 
phase2.c		- the ONLY file you have to modify
phase2.h		- header file defines the functions you have to complete
simulator.c		- main function of the simulator
simulator_linux		- reference simulator compiled in linux1 of our department
Readme.txt		- file you are reading now
phase 2.pdf		- project specification
asm_linux		- assembler for Linux on the Linux servers of our department
case.asm		- sample code for simulator
case.asc		- sample code for simulator in binary
no_branch.asm		- another sample code for simulator
no_branch.asc		- another sample code for simulator in binary
bz162.zip		- a binary editor for you to add invalid instructions
no_branch_invalid.asc	- sample invalid code for simulator in binary