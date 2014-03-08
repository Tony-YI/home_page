/********* Start - Do not touch **********/
#include "phase3_component_state.h"

// Define Constants
#define OPTI0N 4
#define STUDENT_ID_LENGTH 8
#define CHOICE_OPCODE 0
#define CHOICE_JUMP 1
#define CHOICE_REGISTER 2
#define CHOICE_PROGRAM 3
#define OPCODE_CHOICE 4
#define JUMP_CHOICE 4
#define REGISTER_CHOICE 2
#define PROGRAM_CHOICE 5

#define PCINIT 0x0000
#define RA_INDEX 1
#define WORD_SIZE 4
#define MAX_REG_SIZE 32
#define MEM_SIZE (65536 >> 2)
#define MEM(addr) (Mem[addr >> 2])

// Define and Allocate storage for the Memory and Registers
static unsigned Mem[MEM_SIZE];
static unsigned Reg[MAX_REG_SIZE];

const char RegName[MAX_REG_SIZE][6] = {
		"$zero", "$ra", "$fp", "$sp", "$hi", "$lo", "$s1", "$s2", "$s3", "$s4",
			"$s5", "$s6", "$s7", "$s8", "$s9", "$s10", "$s11", "$s12",
			"$s13", "$s14", "$s15", "$s16", "$s17", "$s18", "$s19", "$s20",
			"$s21", "$s22", "$s23", "$s24", "$s25", "$s26"};

// Set up Global Variables for storing component states
Address_Adder_Unit Address_Adder;
ALU_Unit ALU;
ALU_Control_Unit ALU_Control;
AND_Gate_Unit AND_Gate;
Control_Unit Control;
Data_Memory_Unit Data_Memory;
Instruction_Memory_Unit Instruction_Memory;
Instruction_Partitioner_Unit Instruction_Partitioner;
Mux_1_Unit Mux_1;
Mux_2_Unit Mux_2;
Mux_3_Unit Mux_3;
Mux_4_Unit Mux_4;
Mux_5_Unit Mux_5;
Mux_PC_Unit Mux_PC;
PC_Adder_Unit PC_Adder;
Program_Counter_Unit Program_Counter;
Registers_Unit Registers;
Two_bits_Shifter_Unit Two_bits_Shifter;

Hazard_Detection_Unit_Unit Hazard_Detection_Unit;
IFID_Unit IFID;
IDEX_Unit IDEX;
EXMEM_Unit EXMEM;
MEMWB_Unit MEMWB;
Stall_Mux_Unit Stall_Mux;
Mux_A_Unit Mux_A;
Mux_B_Unit Mux_B;
Forwarding_Unit_Unit Forwarding_Unit;

void address_adder();
void alu();
void alu_control();
void and_gate();
void control();
void data_memory();
int instruction_memory();
void instruction_partitioner();
void mux_1();
void mux_2();
void mux_3();
void mux_4();
void mux_5();
void mux_PC();
void pc_adder();
void program_counter();
void registers_read();
void registers_write();
void two_bits_shifter();
void hazard_detection();
void IFID_flip();
void IDEX_flip();
void EXMEM_flip();
void MEMWB_flip();
void stall_mux();
void mux_A();
void mux_B();
void forwarding();
/********* END - Do not touch **********/