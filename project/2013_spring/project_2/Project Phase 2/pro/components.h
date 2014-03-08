/*
 * CSC3420 Project Phase 2
 * Single-cycle simulator
 *
 * This is the header file that specifies the inputs, outputs and control signals of each components
 * Some constant and Alias is define is this file as well.
 * You are suggested to read this file together with Figure 1 and Figure 2 in Phase 2 Specification.
 *
 * !!! Do NOT modify this file, you have to submit phase2.c ONLY. !!!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Constants and Alias
#define PCINIT 0x0000
#define WORD_SIZE 4
#define ZERO_INDEX 0
#define RA_INDEX 1
#define HI_INDEX 4
#define LO_INDEX 5
#define MAX_REG_SIZE 32
#define MEM_PHY_SIZE 65536
#define MEM_SIZE (MEM_PHY_SIZE >> 2)
#define MEM(addr) (Mem[addr >> 2])

// Define and Allocate storage for the Memory and Registers
unsigned Mem[MEM_SIZE];
unsigned Reg[MAX_REG_SIZE];

typedef struct _Program_Counter {
	// Input
	unsigned *Next;
	// Output
	unsigned PC;
} Program_Counter;

typedef struct _PC_Adder {
	// Input
	unsigned D1;
	unsigned *D2;
	// Output
	unsigned RST;
} PC_Adder;

typedef struct _Instruction_Memory {
	// Input
	unsigned *Address;
	// Output
	unsigned Instruction;
} Instruction_Memory;

typedef struct _Instruction_Partitioner {
	// Input
	unsigned *Instruction;
	// Output
	unsigned Opcode;
	unsigned Rs;
	unsigned Rt;
	unsigned Rd;
	unsigned Offset_imm;
	unsigned Target;
	unsigned Funct;
} Instruction_Partitioner;

typedef struct _Control {
	// Input
	unsigned *Opcode;
	// Output Control Signals
	char MemWrite;
	char Branch;
	char Jump;
	char MemToReg;
	char ALUSrc;
	char ALUOp;
	char RegWrite;
	char IType;
	char JumpLink;
} Control;

typedef struct _Register {
	// Input
	unsigned *Rs;
	unsigned *Rt;
	unsigned *Rd;
	unsigned *WData;
	unsigned *WData64;
	// Control Signals
	char *JumpLink;
	char *Data64;
	char *IType;
	char *RegWrite;
	// Output
	unsigned RsData;
	unsigned RtData;
} Register;

typedef struct _ALU_Control {
	// Input
	unsigned *Funct;
	// Control Signals
	char *ALUOp;
	// Output
	unsigned AF;
	// Output Copntrol Signals
	char Data64;
	char JumpReg;
} ALU_Control;

typedef struct _ALU {
	// Input
	unsigned *AData1;
	unsigned *AData2;
	// Control Signals
	unsigned *AF;
	// Output
	unsigned Result;
	unsigned Result64;
	// Output Copntrol Signals
	char Zero;
} ALU;

typedef struct _AND_Gate {
	// Input Control Signals
	char *D1;
	char *D2;
	// Output Control Signal
	char Output;
} AND_Gate;

typedef struct _OR_Gate {
	// Input Control Signals
	char *D1;
	char *D2;
	// Output Control Signal
	char Output;
} OR_Gate;

typedef struct _Data_Memory {
	// Input
	unsigned *Address;
	unsigned *WData;
	// Control Signal
	char *MemWrite;
	// Output
	unsigned RData;
} Data_Memory;

typedef struct _Sign_Extend {
	// Input
	unsigned *In;
	// Output
	unsigned Out;
} Sign_Extend;

typedef struct _Twobit_Shifter {
	// Input
	unsigned *In;
	// Output
	unsigned Out;
} Twobit_Shifter;

typedef struct _Address_Adder {
	// Input
	unsigned *D1;
	unsigned *D2;
	// Output
	unsigned RST;
} Address_Adder;

typedef struct _Mux {
	// Input
	unsigned *D0;
	unsigned *D1;
	// COntrol Signal
	char *Control;
	// Output
	unsigned RST;
} Mux;

// The followings are the components for constructing Mult_Div unit
typedef struct _Register32 {
	// Input
	unsigned In;;
	// Output
	unsigned Content;
} Register32;

typedef struct _Sub_ALU {
	// Input;
	unsigned *D1;
	unsigned *D2;
	unsigned *ToDo;
	// Output
	unsigned RST;
} Sub_ALU;

typedef struct _Shifter64 {
	// Input
	unsigned In;
	unsigned *InAcc;
	// Control Signals
	char *ShiftRight;
	char *ShiftLeft;
	char *Write;
	// Output
	unsigned Left;
	unsigned Right;
} Shifter64;

typedef struct _Test_Control {
	// Input
	unsigned *Left;
	unsigned *Right;
	unsigned AF;
	// Control Signal
	char ShiftRight;
	char ShiftLeft;
	char Write;
	// Output
	unsigned ToDo; 
} Test_Control;
