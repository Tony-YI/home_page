/********* Start - Do not touch **********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct for storing the states of the Address Adder
typedef struct _addressadder {
	// Input
	unsigned *Data_1;
	unsigned *Data_2;
	
	// Output
	unsigned Result;
} Address_Adder_Unit;

// Struct for storing the states of the ALU
typedef struct _alu {
	// Input
	unsigned *ALU_Data_1;
	unsigned *ALU_Data_2;
	
	// Output
	unsigned Zero;
	unsigned ALU_Result;
	
	// Control Signal
	unsigned *Funct;
} ALU_Unit;

// Struct for storing the states of the ALU Control
typedef struct _alucontrol {
	// Input
	unsigned *Input;
	
	// Output
	unsigned Result;
	
	// Control Signal
	unsigned *Control;
} ALU_Control_Unit;

// Struct for storing the states of the AND Gate
typedef struct _andgate {
	// Input
	unsigned *Input_1;
	unsigned *Input_2;
	
	// Output
	unsigned Result;
} AND_Gate_Unit;

// Struct for storing the states of the Control Unit
typedef struct _control {
	// Input
	unsigned *Input;
	
	// Output
	unsigned ALUOp;
	unsigned ALUSrc;
	unsigned Branch;
	unsigned Jump;
	unsigned JumpReg;
	unsigned JumpLink;
	unsigned MemToReg;
	unsigned MemRead;
	unsigned MemWrite;
	unsigned RegWrite;
	unsigned RegSrc;
} Control_Unit;

// Struct for storing the states of the Data Memory
typedef struct _datamemory {
	// Input
	unsigned *Data_Address;
	unsigned *Write_Data;
	
	// Output
	unsigned Read_Data;
	
	// Control Signal
	unsigned *Control_Write;
	unsigned *Control_Read;
} Data_Memory_Unit;

// Struct for storing the states of the Instruction Memory
typedef struct _instructionmemory {
	// Input
	unsigned *Read_Address;
	
	// Output
	unsigned Instruction;
} Instruction_Memory_Unit;

// Struct for storing the states of the Instruction Partitioner
typedef struct _instructionpartitioner {
	// Input
	unsigned *Input;
	
	// Output
	unsigned Opcode;
	unsigned Rs;
	unsigned Rt;
	unsigned Rd;
	unsigned Funct;
	unsigned Constant_Offset;
	unsigned Target;
} Instruction_Partitioner_Unit;

// Struct for storing the states of the Mux 1
typedef struct _mux1 {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	
	// Output
	unsigned Result;

	// Control
	unsigned *Control;
} Mux_1_Unit;

// Struct for storing the states of the Mux 2
typedef struct _mux2 {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	
	// Output
	unsigned Result;

	// Control
	unsigned *Control;
} Mux_2_Unit;

// Struct for storing the states of the Mux 3
typedef struct _mux3 {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	
	// Output
	unsigned Result;

	// Control
	unsigned *Control;
} Mux_3_Unit;

// Struct for storing the states of the Mux 4
typedef struct _mux4 {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	
	// Output
	unsigned Result;

	// Control
	unsigned *Control;
} Mux_4_Unit;

// Struct for storing the states of the Mux 5
typedef struct _mux5 {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	
	// Output
	unsigned Result;

	// Control
	unsigned *Control;
} Mux_5_Unit;



// Struct for storing the states of the PC Adder
typedef struct _pcadder {
	// Input
	unsigned D1;
	unsigned *D2;
	
	// Output
	unsigned Result;
} PC_Adder_Unit;

// Struct for storing the states of the Program Counter
typedef struct _programcounter {
	// Input
	unsigned *Input;
	
	// Output
	unsigned PC;
	
	//Control
	//unsigned *Control;
} Program_Counter_Unit;

// Struct for storing the states of the Registers
typedef struct _registers {
	// Input
	unsigned *Read_Register_1;
	unsigned *Read_Register_2;
	unsigned *Write_Register;
	unsigned *Write_Data;
	
	// Output
	unsigned Read_Data_1;
	unsigned Read_Data_2;

	// Control Signal
	unsigned *Control_RegWrite;
} Registers_Unit;

// Struct for storing the states of the Two-bits Shifter
typedef struct _twobitsshifter {
	// Input
	unsigned *Input;
	
	// Output
	unsigned Result;
} Two_bits_Shifter_Unit;

// Struct for storing the states of the Hazard detection unit
typedef struct _hazardetectionunit {
	// Input
	unsigned *IFID_Rsrc1;
	unsigned *IFID_Rsrc2;
	unsigned *IDEX_Rd;
	unsigned *IDEX_MemRead;

	// Output
	unsigned Stall;
	unsigned IFID_Hold;
	unsigned PC_Hold;
} Hazard_Detection_Unit_Unit;

// Struct for storing the states of the IF/ID register
typedef struct _IFIDunit {
	// Input
	unsigned *Input_PC_Address;
	unsigned *Input_Instruction;
	
	// Output
	unsigned Output_PC_Address;
	unsigned Output_Instruction;
	
	//Flush
	unsigned *Flush;
	unsigned *Flush_2;
	unsigned *Hold;
} IFID_Unit;

// Struct for storing the states of the ID/EX register
typedef struct _IDEXunit {
	// Input
	unsigned *Input_Branch;
	unsigned *Input_Branch_Address;
	//EX
	unsigned *Input_Funct;
	unsigned *Input_ALUOp;
	unsigned *Input_ALUSrc;
	//M
	unsigned *Input_MemRead;
	unsigned *Input_MemWrite;
	//WB
	unsigned *Input_MemToReg;
	unsigned *Input_RegWrite;
	//others
	unsigned *Input_Read_Data_1;
	unsigned *Input_Read_Data_2;
	unsigned *Input_Constant_Offset;
	unsigned *Input_Rsrc1;
	unsigned *Input_Rsrc2;
	unsigned *Input_Rout;		
	
	
	
	// Output
	unsigned Output_Branch;
	unsigned Output_Branch_Address;
	//EX
	unsigned Output_Funct;
	unsigned Output_ALUOp;
	unsigned Output_ALUSrc;
	//M
	unsigned Output_MemRead;
	unsigned Output_MemWrite;
	//WB
	unsigned Output_MemToReg;
	unsigned Output_RegWrite;
	//others
	unsigned Output_Read_Data_1;
	unsigned Output_Read_Data_2;
	unsigned Output_Constant_Offset;
	unsigned Output_Rsrc1;
	unsigned Output_Rsrc2;
	unsigned Output_Rout;
	
	//Flush
	unsigned *Flush;
} IDEX_Unit;

// Struct for storing the states of the EX/MEM register
typedef struct _EXMEMunit {
	// Input
	//M
	unsigned *Input_MemRead;
	unsigned *Input_MemWrite;
	//WB
	unsigned *Input_MemToReg;
	unsigned *Input_RegWrite;
	//others
	unsigned *Input_ALUout;
	unsigned *Input_Write_Data;
	unsigned *Input_Rd;
	
	// Output
	//M
	unsigned Output_MemRead;
	unsigned Output_MemWrite;
	//WB
	unsigned Output_MemToReg;
	unsigned Output_RegWrite;
	//others
	unsigned Output_ALUout;
	unsigned Output_Write_Data;
	unsigned Output_Rd;
	
	//Control
	//unsigned *Control;
} EXMEM_Unit;

// Struct for storing the states of the MEM/WB register
typedef struct _MEMWBunit {
	// Input
	//WB
	unsigned *Input_MemToReg;
	unsigned *Input_RegWrite;
	//others
	unsigned *Input_DataOut;
	unsigned *Input_ALUout;
	unsigned *Input_Rd;
	
	// Output
	//WB
	unsigned Output_MemToReg;
	unsigned Output_RegWrite;
	//others
	unsigned Output_DataOut;
	unsigned Output_ALUout;
	unsigned Output_Rd;
	
	//Control
	//unsigned *Control;
} MEMWB_Unit;

// Struct for storing the states of the Stall Mux
typedef struct _stallmux {
	// Input
	unsigned *Input_Branch;
	//EX
	unsigned *Input_Funct;
	unsigned *Input_ALUOp;
	unsigned *Input_ALUSrc;
	//M
	unsigned *Input_MemRead;
	unsigned *Input_MemWrite;
	//WB
	unsigned *Input_MemToReg;
	unsigned *Input_RegWrite;
	
	// Output
	unsigned Output_Branch;
	//EX
	unsigned Output_Funct;
	unsigned Output_ALUOp;
	unsigned Output_ALUSrc;
	//M
	unsigned Output_MemRead;
	unsigned Output_MemWrite;
	//WB
	unsigned Output_MemToReg;
	unsigned Output_RegWrite;
	
	//Control
	unsigned *Control;
} Stall_Mux_Unit;

// Struct for storing the states of the Mux A
typedef struct _muxA {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	unsigned *Input_2;
	
	// Output
	unsigned Result;
	
	//Control
	unsigned *Control;
} Mux_A_Unit;

// Struct for storing the states of the Mux B
typedef struct _muxB {
	// Input
	unsigned *Input_0;
	unsigned *Input_1;
	unsigned *Input_2;
	
	// Output
	unsigned Result;
	
	//Control
	unsigned *Control;
} Mux_B_Unit;

// Struct for storing the states of the Forwarding Unit
typedef struct _forwardingunit {
	// Input
	unsigned *IDEX_Rsrc1;
	unsigned *IDEX_Rsrc2;
	unsigned *EXMEM_Rd;
	unsigned *EXMEM_RegWrite;
	unsigned *MEMWB_Rd;
	unsigned *MEMWB_RegWrite;

	// Output
	unsigned ForwardingA;
	unsigned ForwardingB;
} Forwarding_Unit_Unit;

// Struct for storing the states of the Mux PC
typedef struct _muxPC {
	// Input
	unsigned *Normal_Address;
	unsigned *Stall_Address;
	
	// Output
	unsigned Final_Address;

	// Control
	unsigned *Control;
} Mux_PC_Unit;
/********* END - Do not touch **********/
