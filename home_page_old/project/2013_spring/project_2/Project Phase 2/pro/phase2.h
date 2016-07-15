/*
 * CSC3420 Project Phase 2
 * Single-cycle simulator
 *
 * This is the header file that specifies the component methods that you have to implement.
 *
 * !!! Do NOT modify this file, you have to submit phase2.c ONLY. !!!
 *
 */

#include <stdio.h>

// Example, please refer to phase2.c
void run_program_counter(unsigned Next, unsigned *PC);

// Complete the following functions in phase2.c
void read_machine_code_file(FILE *input_file);
void run_pc_adder(unsigned D1, unsigned D2, unsigned *RST);
void run_instruction_memory(unsigned Address, unsigned *Instruction);
void run_instruction_partitioner(unsigned Instruction, unsigned *Opcode, unsigned *Rs, unsigned *Rt, unsigned *Rd, unsigned *Offset_imm, unsigned *Target, unsigned *Funct);
int run_control(unsigned Opcode, char *MemWrite, char *Branch, char *Jump, char *MemToReg, char *ALUSrc, char *ALUOp, char *RegWrite, char *IType, char *JumpLink);
void run_register_read(unsigned Rs, unsigned Rt, unsigned Rd, unsigned *RsData, unsigned *RtData);
void run_register_write(unsigned Rs, unsigned Rt, unsigned Rd, unsigned WData, unsigned WData64, char JumpLink, char Data64, char IType, char RegWrite);
int run_alu_control(unsigned Funct, char ALUOp, unsigned *AF, char *Data64, char *JumpReg);
int run_alu(unsigned AData1, unsigned AData2, unsigned AF, char *Zero, unsigned *Result, unsigned *Result64);
void run_and_gate(char D1, char D2, char *Output);
void run_or_gate(char D1, char D2, char *Output);
void run_sign_extend(unsigned In, unsigned *Out);
void run_twobit_shifter(unsigned In, unsigned *Out);
void run_address_adder(unsigned D1, unsigned D2, unsigned *RST);
void run_data_memory(unsigned Address, unsigned WData, char MemWrite, unsigned *RData);
void run_mux(unsigned D0, unsigned D1, char Control, unsigned *RST);
void run_store_sid();