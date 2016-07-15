/********* Start - Do not touch **********/
#include "phase3.h"

int read_machine_code_file(char *filename) {
	int line_of_code;
	unsigned read_instruction;
	FILE *code_fd;
	
	// Open the File Pointer for the Machine Code file and Check for its exist
	// Return -1 if the file is not exist or not accessable
	code_fd = fopen(filename, "rb");
	if (code_fd == NULL) {
		return -1;
	}
	
	// Read the contents from the Machine Code file and store in the variable "Mem"
	line_of_code = 0;
	while (!feof(code_fd)) {
		if (!fread(&read_instruction, 4, 1, code_fd)) {
			break;
		}
		Mem[line_of_code] = 0;
		Mem[line_of_code] = Mem[line_of_code] | ((read_instruction & 0xFF) << 24);
		Mem[line_of_code] = Mem[line_of_code] | ((read_instruction & 0xFF00) << 8);
		Mem[line_of_code] = Mem[line_of_code] | ((read_instruction & 0xFF0000) >> 8);
		Mem[line_of_code] = Mem[line_of_code] | ((read_instruction & 0xFF000000) >> 24);
		line_of_code++;
	}
	
	// Close the File Pointer
	fclose(code_fd);
	return 1;
}
/********* END - Do not touch **********/

// Task 1: Initialize output states in some components to zero
void initialize_register_states() {
	// Example: Initialize the value of program counter to 0x0000
	*Program_Counter.Input = PCINIT;
	// Example: initializing the output of Program Counter to zero
	Program_Counter.PC = 0;
	// Example: initializing the output of Mux_PC to zero
	Mux_PC.Final_Address = 0;
	
	// Your codes here
	//凡是输入从后面指回来的control signal都要init
	/********************First Part********************/
	//init Mux_1
	*Mux_1.Control = 0;//no jump
	//init Mux_2
	*Mux_2.Control = 0;//no branch
	//init Mux_PC
	*Mux_PC.Control = 0;
	/**************************************************/
	
	//init IFID
	*IFID.Flush = 0;//no jump
	*IFID.Flush_2 = 0;//no branch
	*IFID.Hold = 0;//no hazard
	
	/********************Second Part*******************/
	//init Hazar_detection_unit
	*Hazard_Detection_Unit.IDEX_MemRead = 0;
	//init Registers
	*Registers.Control_RegWrite = 0;//no write back signal
	/**************************************************/
	
	//init IDEX
	*IDEX.Flush = 0;//no branch
	
	/*****************Third Part***********************/
	//init Mux_A
	*Mux_A.Control = 0;//no forwarding
	//init Mux_B
	*Mux_B.Control = 0;//no forwaridng
	//init Forwaridng_Unit
	*Forwarding_Unit.EXMEM_RegWrite = 0;
	*Forwarding_Unit.MEMWB_RegWrite = 0;
	/**************************************************/
	
	//init EXMEM
	
	/******************Forth Part**********************/
	/**************************************************/
	
	//init MEMWB
	
	/*******************Fifth Part*********************/
	/**************************************************/
}

// Task 2: Connect the other components - drawing datapaths
void connect_components() {	
	// Example: The following code is written to connect the Instruction Memory Unit to Program Counter Unit
	Instruction_Memory.Read_Address = &Program_Counter.PC;
	// Example: The following code is written to connect the input of the program counter to the output of the Mux_PC
	Program_Counter.Input = &Mux_PC.Final_Address;
	// Your codes here
	
	/************First Part**************/
	//MUX_PC
	Mux_PC.Control = &Hazard_Detection_Unit.PC_Hold;
	Mux_PC.Normal_Address = &Mux_2.Result;
	Mux_PC.Stall_Address = &Program_Counter.PC;
	
	//PC_Adder
	PC_Adder.D1 = 4;
	PC_Adder.D2 = &Program_Counter.PC;
	
	//Mux_1
	Mux_1.Control = &Control.Jump;
	Mux_1.Input_0 = &PC_Adder.Result;
	Mux_1.Input_1 = &Instruction_Partitioner.Target;
	
	//Mux_2
	Mux_2.Control = &AND_Gate.Result;
	Mux_2.Input_0 = &Mux_1.Result;
	Mux_2.Input_1 = &IDEX.Output_Branch_Address;
	/****************************************/
	
	//IFID
	IFID.Flush = &Control.Jump;//if jump == 1, then flush the registers
	IFID.Flush_2 = &AND_Gate.Result;//if and_gate.result == 1, it means branch, then flush the registers
	IFID.Hold = &Hazard_Detection_Unit.IFID_Hold;//hold for bubble
	IFID.Input_Instruction = &Instruction_Memory.Instruction;
	IFID.Input_PC_Address = &PC_Adder.Result;
	
	/**********Second Part*******************/
	//Instructrin_Partitionar
	//******************************no such unit in the graph********************************//
	Instruction_Partitioner.Input = &IFID.Output_Instruction;
	
	//Hazard_Detection_Unit
	Hazard_Detection_Unit.IFID_Rsrc1 = &Instruction_Partitioner.Rs;
	Hazard_Detection_Unit.IFID_Rsrc2 = &Instruction_Partitioner.Rt;
	Hazard_Detection_Unit.IDEX_Rd = &IDEX.Output_Rout;//write back register destination
	Hazard_Detection_Unit.IDEX_MemRead = &IDEX.Output_MemRead;
	
	//Control_Unit
	Control.Input = &IFID.Output_Instruction;
	
	//Address_Adder
	Address_Adder.Data_1 = &IFID.Output_PC_Address;
	Address_Adder.Data_2 = &Two_bits_Shifter.Result;
	
	//Stall_Mux
	Stall_Mux.Control = &Hazard_Detection_Unit.Stall;
	Stall_Mux.Input_ALUOp = &Control.ALUOp;
	Stall_Mux.Input_ALUSrc = &Control.ALUSrc;
	Stall_Mux.Input_Branch = &Control.Branch;
	Stall_Mux.Input_Funct = &Instruction_Partitioner.Funct;
	Stall_Mux.Input_MemRead = &Control.MemRead;
	Stall_Mux.Input_MemToReg = &Control.MemToReg;
	Stall_Mux.Input_MemWrite = &Control.MemWrite;
	Stall_Mux.Input_RegWrite = &Control.RegWrite;
	
	//Two_Bit_Shifter
	Two_bits_Shifter.Input = &Instruction_Partitioner.Constant_Offset;
	
	//Registers
	Registers.Read_Register_1 = &Instruction_Partitioner.Rs;//rs
	Registers.Read_Register_2 = &Instruction_Partitioner.Rt;//rt
	Registers.Write_Data = &Mux_5.Result;
	Registers.Write_Register = &MEMWB.Output_Rd;//rd
	Registers.Control_RegWrite = &MEMWB.Output_RegWrite;
	
	//Mux_3
	Mux_3.Control = &Control.RegSrc;
	Mux_3.Input_0 = &Instruction_Partitioner.Rt;//control signal is 0, then output is rt
	Mux_3.Input_1 = &Instruction_Partitioner.Rd;
	/****************************************/
	
	//IDEX
	//Input
	IDEX.Input_Branch = &Stall_Mux.Output_Branch;
	IDEX.Input_Branch_Address = &Address_Adder.Result;
	//EX
	IDEX.Input_Funct = &Stall_Mux.Output_Funct;
	IDEX.Input_ALUOp = &Stall_Mux.Output_ALUOp;
	IDEX.Input_ALUSrc = &Stall_Mux.Output_ALUSrc;
	//M
	IDEX.Input_MemRead = &Stall_Mux.Output_MemRead;
	IDEX.Input_MemWrite = &Stall_Mux.Output_MemWrite;
	//WB
	IDEX.Input_MemToReg = &Stall_Mux.Output_MemToReg;
	IDEX.Input_RegWrite = &Stall_Mux.Output_RegWrite;
	//others
	IDEX.Input_Read_Data_1 = &Registers.Read_Data_1;//rs
	IDEX.Input_Read_Data_2 = &Registers.Read_Data_2;//rt
	IDEX.Input_Constant_Offset = &Instruction_Partitioner.Constant_Offset;
	IDEX.Input_Rsrc1 = &Instruction_Partitioner.Rs;//rs
	IDEX.Input_Rsrc2 = &Instruction_Partitioner.Rt;//rt
	IDEX.Input_Rout = &Mux_3.Result;//rd
	//Flush
	IDEX.Flush = &AND_Gate.Result;//1 means branch, then flush the registers in IDEX
	
	/***********Third Part*******************/
	//AND_Gate
	AND_Gate.Input_1 = &ALU.Zero;//if rs == rt, ALU.Zero is 0
	AND_Gate.Input_2 = &IDEX.Output_Branch;
	
	//ALU_Control
	ALU_Control.Control = &IDEX.Output_ALUOp;//0 means no ALU operation,1 means does
	ALU_Control.Input = &IDEX.Output_Funct;//using this to determine what the operation is
	
	//ALU
	ALU.ALU_Data_1 = &Mux_A.Result;
	ALU.ALU_Data_2 = &Mux_4.Result;
	ALU.Funct = &ALU_Control.Result;//determine what operation will do
	
	//Mux_A
	Mux_A.Control = &Forwarding_Unit.ForwardingA;
	Mux_A.Input_0 = &IDEX.Output_Read_Data_1;//no forwarding
	Mux_A.Input_1 = &EXMEM.Output_ALUout;//one forwarding, forwarding from step Mem
	Mux_A.Input_2 = &Mux_5.Result;//two forwarding, forwarding from step WB
	
	//Mux_B
	Mux_B.Control = &Forwarding_Unit.ForwardingB;
	Mux_B.Input_0 = &IDEX.Output_Read_Data_2;//no forwarding
	Mux_B.Input_1 = &EXMEM.Output_ALUout;//one forwarding, forwarding from step Mem
	Mux_B.Input_2 = &Mux_5.Result;//two forwarding, forwarding from step WB
	
	//Mux_4
	Mux_4.Control = &IDEX.Output_ALUSrc;//0 from register, 1 from immi
	Mux_4.Input_0 = &IDEX.Output_Constant_Offset;
	Mux_4.Input_1 = &Mux_B.Result;
	
	//Forwarding_Unit
	Forwarding_Unit.IDEX_Rsrc1 = &IDEX.Output_Rsrc1;
	Forwarding_Unit.IDEX_Rsrc2 = &IDEX.Output_Rsrc2;
	Forwarding_Unit.EXMEM_Rd = &EXMEM.Output_Rd;
	Forwarding_Unit.EXMEM_RegWrite = &EXMEM.Output_RegWrite;
	Forwarding_Unit.MEMWB_Rd = &MEMWB.Output_Rd;
	Forwarding_Unit.MEMWB_RegWrite = &MEMWB.Output_RegWrite;
	/****************************************/
	
	//EXMEM
	//M
	EXMEM.Input_MemRead = &IDEX.Output_MemRead;
	EXMEM.Input_MemWrite = &IDEX.Output_MemWrite;
	//WB
	EXMEM.Input_MemToReg = &IDEX.Output_MemToReg;
	EXMEM.Input_RegWrite = &IDEX.Output_RegWrite;
	//others
	EXMEM.Input_ALUout = &ALU.ALU_Result;//for sw, this is the memory address
	EXMEM.Input_Write_Data = &Mux_B.Result;//for sw, this is the data wrote into the memory
	EXMEM.Input_Rd = &IDEX.Output_Rout;
	
	/************Fourth Part*****************/
	//Data_Memory
	Data_Memory.Data_Address = &EXMEM.Output_ALUout;
	Data_Memory.Write_Data = &EXMEM.Output_Write_Data;
	Data_Memory.Control_Write = &EXMEM.Output_MemWrite;
	Data_Memory.Control_Read = &EXMEM.Output_MemRead;
	/****************************************/
	
	//MEMWB
	//WB
	MEMWB.Input_MemToReg = &EXMEM.Output_MemToReg;
	MEMWB.Input_RegWrite = &EXMEM.Output_RegWrite;
	//others
	MEMWB.Input_DataOut = &Data_Memory.Read_Data;
	MEMWB.Input_ALUout = &EXMEM.Output_ALUout;
	MEMWB.Input_Rd = &EXMEM.Output_Rd;
	
	/*******************Fifth Part************/
	//Mux_5
	Mux_5.Control = &MEMWB.Output_MemToReg;//0 means alu_result to register, 1 means mem_to_register
	Mux_5.Input_0 = &MEMWB.Output_ALUout;
	Mux_5.Input_1 = &MEMWB.Output_DataOut;
	/*****************************************/
}

// Task 4: Please implement the other component units' methods.
/********* Start - Do not touch **********/
// Example: In the Program Count method, it copies the input as output
void program_counter() {
	Program_Counter.PC = *Program_Counter.Input;
}
// Example: In the Instruction memory method, it reads the instruction memory regarding the input address
// If end of program is determined, 1 is outputted. Otherwise, 0 is outputted. 
int instruction_memory() {
	Instruction_Memory.Instruction = MEM(*Instruction_Memory.Read_Address);
	if (Instruction_Memory.Instruction == 0){ 
		return 1;
	}
	return 0;
}
/********* END - Do not touch **********/

void address_adder() {
	// Your codes here
	//for calculating the branch address
	Address_Adder.Result = (*Address_Adder.Data_1) + (*Address_Adder.Data_2);
}

void alu() {
	// Your codes here
	switch(*ALU.Funct)
		{
			case 0x11://add
			{
				ALU.ALU_Result = (*ALU.ALU_Data_1) + (*ALU.ALU_Data_2);
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 0x12://sub
			{
				ALU.ALU_Result = (*ALU.ALU_Data_1) - (*ALU.ALU_Data_2);
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 0x18://mult
			{
				ALU.ALU_Result = (*ALU.ALU_Data_1) * (*ALU.ALU_Data_2);
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			case 0x1a://div
			{
				ALU.ALU_Result = (*ALU.ALU_Data_1) / (*ALU.ALU_Data_2);
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 0x2a://and
			{
				ALU.ALU_Result = ((*ALU.ALU_Data_1) & (*ALU.ALU_Data_2));
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE  
			}break;
			
			case 0x2b://or
			{
				ALU.ALU_Result = ((*ALU.ALU_Data_1) | (*ALU.ALU_Data_2));
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 1://sllv
			{
				ALU.ALU_Result = ((*ALU.ALU_Data_2) << (*ALU.ALU_Data_1));
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 2://srlv
			{
				ALU.ALU_Result = ((*ALU.ALU_Data_2) >> (*ALU.ALU_Data_1));
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 0x25://slt
			{
				ALU.ALU_Result = (((*ALU.ALU_Data_1) < (*ALU.ALU_Data_2))? 1 : 0);
				ALU.Zero =  (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
			
			case 0://addi, or sw, or lw, or bnq, 3 is determined by yourself
			{
				ALU.ALU_Result = (*ALU.ALU_Data_1) + (*ALU.ALU_Data_2);
				ALU.Zero = (((*ALU.ALU_Data_1) == (*ALU.ALU_Data_2))? 0 : 1);//CARE 
			}break;
		}
}

void alu_control() {
	// Your codes here
	if(*ALU_Control.Control == 1)//Instruction OPCode is 0x0f, means add, sub, mult, div, and, or, sllv, srlv,slt
	{
		ALU_Control.Result = *ALU_Control.Input;
	}
	else if(*ALU_Control.Control == 0)//Instruction OPCode is not 0x0f, means addi, lw, sw, bnq, j, but j doesn't need to use ALU
	{
		ALU_Control.Result = 0;//0 is determined by yourself
	}
}

void and_gate() {
	// Your codes here
	if(*AND_Gate.Input_1 == 1 && *AND_Gate.Input_2 == 1)
	{
		AND_Gate.Result = 1;
	}
	else
	{
		AND_Gate.Result = 0;
	}
}

void control() {
	// Your codes here
	if(((*Control.Input) >> 26) == 0x0f)//OPCode, R-type
	{
		if((((*Control.Input) & 0x0000003f) == 0x18) || (((*Control.Input) & 0x0000003f) == 0x1a))//funct, mult or div
		{
			Control.ALUOp = 1;//Instruction OPCode is 0x0f
			Control.ALUSrc = 0;//ALU source is from register
			Control.Branch = 0;//no Branch
			Control.Jump =0;//no Jump
			Control.JumpReg = 0;//no Jump Register
			Control.JumpLink = 0;//no Jump and Link
			Control.MemToReg = 0;//no data is read from memory to register
			Control.MemRead = 0;//no Memory Read
			Control.MemWrite = 0;//no Memory Write
			Control.RegWrite = 1;//result write back to register
			Control.RegSrc = 0;//register destination $lo
		}
		else
		{
			Control.ALUOp = 1;//Instruction OPCode is 0x0f
			Control.ALUSrc = 0;//ALU source is from register
			Control.Branch = 0;//no Branch
			Control.Jump =0;//no Jump
			Control.JumpReg = 0;//no Jump Register
			Control.JumpLink = 0;//no Jump and Link
			Control.MemToReg = 0;//no data is read from memory to register
			Control.MemRead = 0;//no Memory Read
			Control.MemWrite = 0;//no Memory Write
			Control.RegWrite = 1;//result write back to register
			Control.RegSrc = 0;//register destination is rd
		}
	}
	else if(((*Control.Input) >> 26) == 0x1a)//OPCode, lw
	{
		Control.ALUOp = 0;//Instruction OPCode is not 0x0f
		Control.ALUSrc = 1;//ALU source is from 16-bit immi
		Control.Branch = 0;//no Branch
		Control.Jump =0;//no Jump
		Control.JumpReg = 0;//no Jump Register
		Control.JumpLink = 0;//no Jump and Link
		Control.MemToReg = 1;//data is read from memory to register
		Control.MemRead = 1;//Memory Read
		Control.MemWrite = 0;//no Memory Write
		Control.RegWrite = 1;//result write back to register
		Control.RegSrc = 1;//register destination is rt
	}
	else if(((*Control.Input) >> 26) == 0x1b)//OPCode, sw
	{
		Control.ALUOp = 0;//Instruction OPCode is not 0x0f
		Control.ALUSrc = 1;//ALU source is from 16-bit immi
		Control.Branch = 0;//no Branch
		Control.Jump =0;//no Jump
		Control.JumpReg = 0;//no Jump Register
		Control.JumpLink = 0;//no Jump and Link
		Control.MemToReg = 0;//no data is read from memory to register
		Control.MemRead = 0;//no Memory Read
		Control.MemWrite = 1;//Memory Write
		Control.RegWrite = 0;//no result is writen back to register
		Control.RegSrc = 0;//register destination doesn't care,but will change by the previous step
	}
	else if(((*Control.Input) >> 26) == 1)//OPCode, addi
	{
		Control.ALUOp = 0;//Instruction OPCode is not 0x0f
		Control.ALUSrc = 1;//ALU source is from 16-bit immi
		Control.Branch = 0;//no Branch
		Control.Jump =0;//no Jump
		Control.JumpReg = 0;//no Jump Register
		Control.JumpLink = 0;//no Jump and Link
		Control.MemToReg = 0;//no data is read from memory to register
		Control.MemRead = 0;//no Memory Read
		Control.MemWrite = 0;//no Memory Write
		Control.RegWrite = 1;//result is writen back to register
		Control.RegSrc = 1;//register destination is rt
	}
	else if(((*Control.Input) >> 26) == 2)//OPCode, bne
	{
		Control.ALUOp = 0;//Instruction OPCode is not 0x0f
		Control.ALUSrc = 0;//ALU source is from register
		Control.Branch = 1;//Branch
		Control.Jump = 0;//Jump doesn't care
 		Control.JumpReg = 0;//no Jump Register
		Control.JumpLink = 0;//no Jump and Link
		Control.MemToReg = 0;//no data is read from memory to register
		Control.MemRead = 0;//no Memory Read
		Control.MemWrite = 0;//no Memory Write
		Control.RegWrite = 0;//no result is writen back to register
		Control.RegSrc = 0;//register destination doesn't care
	}
	else if(((*Control.Input) >> 26) == 3)//OPCode, j
	{
		Control.ALUOp = 0;//Instruction OPCode is not 0x0f
		Control.ALUSrc = 0;//ALU source is from 26-bit target
		Control.Branch = 0;//no Branch
		Control.Jump = 1;//Jump
		Control.JumpReg = 0;//no Jump Register
		Control.JumpLink = 0;//no Jump and Link
		Control.MemToReg = 0;//no data is read from memory to register
		Control.MemRead = 0;//no Memory Read
		Control.MemWrite = 0;//no Memory Write
		Control.RegWrite = 0;//no result is writen back to register
		Control.RegSrc = 0;//register destination doesn't care
	}
	else//avalid OPCode
	{
		Control.ALUOp = 0;//Instruction OPCode is not 0x0f
		Control.ALUSrc = 0;//ALU source is from 26-bit target
		Control.Branch = 0;//no Branch
		Control.Jump = 0;//no Jump
		Control.JumpReg = 0;//no Jump Register
		Control.JumpLink = 0;//no Jump and Link
		Control.MemToReg = 0;//no data is read from memory to register
		Control.MemRead = 0;//no Memory Read
		Control.MemWrite = 0;//no Memory Write
		Control.RegWrite = 0;//no result is writen back to register
		Control.RegSrc = 0;//register destination doesn't care
	}
}

void data_memory() {
	// Your codes here
	//write memory FIRST
	if(*Data_Memory.Control_Write == 1)
	{
		Mem[*Data_Memory.Data_Address >> 2] = *Data_Memory.Write_Data;
	}
	//read memory
	if(*Data_Memory.Control_Read == 1)
	{
		Data_Memory.Read_Data = Mem[*Data_Memory.Data_Address >> 2];//don't forget the two bit right shift
	}
}

void instruction_partitioner() {
	// Your codes here
	Instruction_Partitioner.Opcode = (*Instruction_Partitioner.Input) >> 26 & 0x0000003f;
	Instruction_Partitioner.Rs = ((*Instruction_Partitioner.Input) >> 21) & 0x0000001f;
	Instruction_Partitioner.Rt = ((*Instruction_Partitioner.Input) >> 16) & 0x0000001f;
	Instruction_Partitioner.Rd = ((*Instruction_Partitioner.Input) >> 11) & 0x0000001f;
	Instruction_Partitioner.Funct = (*Instruction_Partitioner.Input) & 0x0000003f;
	if((((*Instruction_Partitioner.Input) >> 15) & 0x00000001) == 1)//since there is no Signed-Extend Unit in the .h file
	{
		Instruction_Partitioner.Constant_Offset = ((*Instruction_Partitioner.Input) & 0x0000ffff) | 0xffff0000;
	}
	else if((((*Instruction_Partitioner.Input) >> 15) & 0x00000001) == 0)
	{
		Instruction_Partitioner.Constant_Offset = ((*Instruction_Partitioner.Input) & 0x0000ffff);
	}
	Instruction_Partitioner.Target = (*Instruction_Partitioner.Input) & 0x03ffffff;
}

void mux_1() {
	// Your codes here
	//control=0, no jump,else jump
	if( (*Mux_1.Control) == 0 )
	{
		Mux_1.Result = *Mux_1.Input_0;
	}
	else if( (*Mux_1.Control) == 1 )
	{
		Mux_1.Result = *Mux_1.Input_1;
	}
}

void mux_2() {
	// Your codes here
	//control=0, no branch,else branch
	if( (*Mux_2.Control) == 0 )
	{
		Mux_2.Result = *Mux_2.Input_0;
	}
	else if( (*Mux_2.Control) == 1 )
	{
		Mux_2.Result = *Mux_2.Input_1;
	}
}

void mux_3() {
	// Your Codes here
	//control=1, register_source is rt; else register_source is rd
	if( (*Mux_3.Control) == 1 )
	{
		Mux_3.Result = *Mux_3.Input_0;
	}
	else if( (*Mux_3.Control) == 0 )
	{
		Mux_3.Result = *Mux_3.Input_1;
	}
}

void mux_4() {
	// Your codes here
	//control=1, ALU_source is immi;else ALU_src is register
	if( (*Mux_4.Control) == 1 )
	{
		Mux_4.Result = *Mux_4.Input_0;
	}
	else if( (*Mux_4.Control) == 0 )
	{
		Mux_4.Result = *Mux_4.Input_1;
	}
}

void mux_5() {
	// Your Codes here
	//control=0, source is from memory;else source is from ALU result
	if( (*Mux_5.Control) == 0 )
	{
		Mux_5.Result = *Mux_5.Input_0;
	}
	else if( (*Mux_5.Control) == 1 )
	{
		Mux_5.Result = *Mux_5.Input_1;
	}
}

void mux_PC() {
	// Your codes here
	//control=1, final address is stalled address;else final address is normal address
	if( (*Mux_PC.Control) == 0 )
	{
		Mux_PC.Final_Address = *Mux_PC.Normal_Address;
	}
	else if( (*Mux_PC.Control) == 1 )
	{
		Mux_PC.Final_Address = *Mux_PC.Stall_Address;
	}
}

void pc_adder() {
	// Your codes here
	PC_Adder.Result = PC_Adder.D1 + (*PC_Adder.D2);
}

void registers_read() {
	// Your codes here
	//RegWrite = 0 is register_read
	if(*Registers.Control_RegWrite == 1)//when we reading the data, some one want to write data into the register, Structural Hazard
										//write first then read
	{
		Reg[*Registers.Write_Register] = *Registers.Write_Data;
		Reg[0] = 0;
		Registers.Read_Data_1 = Reg[*Registers.Read_Register_1];//rs
		Registers.Read_Data_2 = Reg[*Registers.Read_Register_2];//rt
	}
	else if(*Registers.Control_RegWrite == 0)//only read
	{
		Registers.Read_Data_1 = Reg[*Registers.Read_Register_1];//rs
		Registers.Read_Data_2 = Reg[*Registers.Read_Register_2];//rt
	}
}

void registers_write() {
	// Your codes here
	//RegWrite = 1 is register_write
	if(*Registers.Control_RegWrite == 1)
	{
		Reg[*Registers.Write_Register] = *Registers.Write_Data;
	}
	
	Reg[0] = 0;
}

void two_bits_shifter() {
	// Your codes here
	Two_bits_Shifter.Result = (*Two_bits_Shifter.Input) << 2;
}

void hazard_detection() {
	// Your codes here
	//lw r1, 0(r2)
	//sub r4, r1, r3
	//sub r4, r1, r2
	//insert bubble
	if(*Hazard_Detection_Unit.IDEX_MemRead && ((*Hazard_Detection_Unit.IDEX_Rd ==
			*Hazard_Detection_Unit.IFID_Rsrc1) || (*Hazard_Detection_Unit.IDEX_Rd == *Hazard_Detection_Unit.IFID_Rsrc2)) )
	{
		Hazard_Detection_Unit.IFID_Hold = 1;
		Hazard_Detection_Unit.Stall = 1;
		Hazard_Detection_Unit.PC_Hold = 1;
	}
	else
	{
		Hazard_Detection_Unit.IFID_Hold = 0;
		Hazard_Detection_Unit.Stall = 0;
		Hazard_Detection_Unit.PC_Hold = 0;
	}
}

void IFID_flip() {
	// Your codes here
	if(*IFID.Flush)//JUMP FLUSH
	{
		IFID.Output_Instruction = 0;
		IFID.Output_PC_Address = *IFID.Input_PC_Address;//in jump flush, also no need to flush PC_Address!!
	}
	if(*IFID.Flush_2)//BRANCH FLUSH
	{
		IFID.Output_Instruction = 0;
		IFID.Output_PC_Address = *IFID.Input_PC_Address;//in branch flush, no need to flush the PC_Address!
	}
	if(*IFID.Hold)//perform hold
	{
		IFID.Output_Instruction = IFID.Output_Instruction;
		IFID.Output_PC_Address = *IFID.Input_PC_Address;//in hold, no need to hold the PC_Address
	}
	if( !(*IFID.Flush) && !(*IFID.Flush_2) && !(*IFID.Hold) )//normal
	{
		IFID.Output_Instruction = *IFID.Input_Instruction;
		IFID.Output_PC_Address = *IFID.Input_PC_Address;
	}
}

void IDEX_flip() {
	// Your codes here
	if(*IDEX.Flush)//flush
	{
		IDEX.Output_ALUOp = 0;
		IDEX.Output_ALUSrc = 0;
		IDEX.Output_Branch = 0;
		IDEX.Output_Branch_Address = 0;
		IDEX.Output_Constant_Offset = 0;
		IDEX.Output_Funct = 0;
		IDEX.Output_MemRead = 0;
		IDEX.Output_MemToReg = 0;
		IDEX.Output_MemWrite = 0;
		IDEX.Output_Read_Data_1 = 0;
		IDEX.Output_Read_Data_2 = 0;
		IDEX.Output_RegWrite = 0;
		IDEX.Output_Rout = 0;
		IDEX.Output_Rsrc1 = 0;
		IDEX.Output_Rsrc2 = 0;
	}
	else
	{
		IDEX.Output_ALUOp = *IDEX.Input_ALUOp;
		IDEX.Output_ALUSrc = *IDEX.Input_ALUSrc;
		IDEX.Output_Branch = *IDEX.Input_Branch;
		IDEX.Output_Branch_Address = *IDEX.Input_Branch_Address;
		IDEX.Output_Constant_Offset = *IDEX.Input_Constant_Offset;
		IDEX.Output_Funct = *IDEX.Input_Funct;
		IDEX.Output_MemRead = *IDEX.Input_MemRead;
		IDEX.Output_MemToReg = *IDEX.Input_MemToReg;
		IDEX.Output_MemWrite = *IDEX.Input_MemWrite;
		IDEX.Output_Read_Data_1 = *IDEX.Input_Read_Data_1;
		IDEX.Output_Read_Data_2 = *IDEX.Input_Read_Data_2;
		IDEX.Output_RegWrite = *IDEX.Input_RegWrite;
		IDEX.Output_Rsrc1 = *IDEX.Input_Rsrc1;
		IDEX.Output_Rsrc2 = *IDEX.Input_Rsrc2;
		IDEX.Output_Rout = *IDEX.Input_Rout;
	}
}

void EXMEM_flip() {
	// Your codes here
	EXMEM.Output_ALUout = *EXMEM.Input_ALUout;
	EXMEM.Output_MemRead = *EXMEM.Input_MemRead;
	EXMEM.Output_MemToReg = *EXMEM.Input_MemToReg;
	EXMEM.Output_MemWrite = *EXMEM.Input_MemWrite;
	EXMEM.Output_Rd = *EXMEM.Input_Rd;
	EXMEM.Output_RegWrite = *EXMEM.Input_RegWrite;
	EXMEM.Output_Write_Data = *EXMEM.Input_Write_Data;
}

void MEMWB_flip() {
	// Your codes here
	MEMWB.Output_ALUout = *MEMWB.Input_ALUout;
	MEMWB.Output_DataOut = *MEMWB.Input_DataOut;
	MEMWB.Output_MemToReg = *MEMWB.Input_MemToReg;
	MEMWB.Output_Rd = *MEMWB.Input_Rd;
	MEMWB.Output_RegWrite = *MEMWB.Input_RegWrite;
}

void stall_mux() {
	// Your codes here
	if(*Stall_Mux.Control)//stalling
	{
		Stall_Mux.Output_ALUOp = 0;
		Stall_Mux.Output_ALUSrc = 0;
		Stall_Mux.Output_Branch = 0;
		Stall_Mux.Output_Funct = 0;
		Stall_Mux.Output_MemRead = 0;
		Stall_Mux.Output_MemToReg = 0;
		Stall_Mux.Output_MemWrite = 0;
		Stall_Mux.Output_RegWrite = 0;
	}
	else if( !(*Stall_Mux.Control) )//no stalling
	{
		Stall_Mux.Output_ALUOp = *Stall_Mux.Input_ALUOp;
		Stall_Mux.Output_ALUSrc = *Stall_Mux.Input_ALUSrc;
		Stall_Mux.Output_Branch = *Stall_Mux.Input_Branch;
		Stall_Mux.Output_Funct = *Stall_Mux.Input_Funct;
		Stall_Mux.Output_MemRead = *Stall_Mux.Input_MemRead;
		Stall_Mux.Output_MemToReg = *Stall_Mux.Input_MemToReg;
		Stall_Mux.Output_MemWrite = *Stall_Mux.Input_MemWrite;
		Stall_Mux.Output_RegWrite = *Stall_Mux.Input_RegWrite;
	}
}

void mux_A() {
	// Your codes here
	if(*Mux_A.Control == 0)
	{
		Mux_A.Result = *Mux_A.Input_0;//no forwarding
	}
	else if(*Mux_A.Control == 1)
	{
		Mux_A.Result = *Mux_A.Input_1;//forwarding from step Mem, one forwarding
	}
	else if(*Mux_A.Control == 2)
	{
		Mux_A.Result = *Mux_A.Input_2;//forwarding from step WB, two forwarding
	}
}

void mux_B() {
	// Your codes here
	if(*Mux_B.Control == 0)
	{
		Mux_B.Result = *Mux_B.Input_0;//no forwarding
	}
	else if(*Mux_B.Control == 1)
	{
		Mux_B.Result = *Mux_B.Input_1;//forwarding from step Mem, one forwarding
	}
	else if(*Mux_B.Control == 2)
	{
		Mux_B.Result = *Mux_B.Input_2;//forwarding from step WB, two forwarding
	}
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
void forwarding() {
	// Your codes here
	//as default, no forwarding
	Forwarding_Unit.ForwardingA = 0;
	Forwarding_Unit.ForwardingB = 0;
	
	if((*Forwarding_Unit.EXMEM_RegWrite) == 1)//has register write operation, then perform one forwarding
	{
		if((*Forwarding_Unit.IDEX_Rsrc1) == (*Forwarding_Unit.EXMEM_Rd))
		{
			Forwarding_Unit.ForwardingA = 1;
		}
		if((*Forwarding_Unit.IDEX_Rsrc2) == (*Forwarding_Unit.EXMEM_Rd))
		{
			Forwarding_Unit.ForwardingB = 1;
		}
	}
	if((*Forwarding_Unit.MEMWB_RegWrite) == 1)//has register write operation, then perform two forwarding
	{
		//add r1, r2, r3
		//sub r1, r1, r3
		//and r6, r1, r7
		if((*Forwarding_Unit.EXMEM_Rd) != (*Forwarding_Unit.MEMWB_Rd))//need to perform two forwarding, else no need to perform two forwarding
		{
			if((*Forwarding_Unit.IDEX_Rsrc1) == (*Forwarding_Unit.MEMWB_Rd))
			{
				Forwarding_Unit.ForwardingA = 2;
			}
			if((*Forwarding_Unit.IDEX_Rsrc2) == (*Forwarding_Unit.MEMWB_Rd))
			{
				Forwarding_Unit.ForwardingB = 2;
			}
		}
	}
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //

// Task 3: Please call the components with the most appropriate sequence
int simulate_Cycle() {
	// In each cycle, the simulator should call each component to perform its job.
	// Your codes here
	
	// !!!!!!!!!!!!!!!!!!!!from step 5 to step 1!!!!!!!!!!!!!!!!!!!!!!!! //
	MEMWB_flip();
	EXMEM_flip();
	IDEX_flip();
	IFID_flip();
	//step 5, WB step//
	mux_5();
	registers_write();
	
	//step 4, M step//
	data_memory();
	//printf("Data_Memory.Control_Read: %d\n", *Data_Memory.Control_Read);
	//printf("Data_Memory.Control_Write: %d\n", *Data_Memory.Control_Write);
	//printf("Data_Memory.Data_Address: %d\n", *Data_Memory.Data_Address);
	//printf("Data_Memory.Write_Data: %d\n", *Data_Memory.Write_Data);
	//printf("Data_Memory.Read_Data: %d\n", Data_Memory.Read_Data);
	
	//step 3, EX step, the sequence of the components DOES CARE//
	alu_control();
	forwarding();
	//printf("Forwarding_Unit.EXMEM_Rd: %d\n", *Forwarding_Unit.EXMEM_Rd);
	//printf("Forwarding_Unit.EXMEM_RegWrite: %d\n", *Forwarding_Unit.EXMEM_RegWrite);
	//printf("Forwarding_Unit.MEMWB_Rd: %d\n", *Forwarding_Unit.MEMWB_Rd);
	//printf("Forwarding_Unit.MEMWB_RegWrite: %d\n", *Forwarding_Unit.MEMWB_RegWrite);
	//printf("Forwarding_Unit.IDEX_Rsrc1: %d\n", *Forwarding_Unit.IDEX_Rsrc1);
	//printf("Forwarding_Unit.IDEX_Rsrc2: %d\n", *Forwarding_Unit.IDEX_Rsrc2);
	//printf("Forwarding_Unit.ForwardingA: %d\n", Forwarding_Unit.ForwardingA);
	//printf("Forwarding_Unit.ForwardingB: %d\n", Forwarding_Unit.ForwardingB);
	mux_A();
	//printf("Mux_A.Control: %d\n", *Mux_A.Control);
	//printf("Mux_A.Input_0: %d\n", *Mux_A.Input_0);
	//printf("Mux_A.Input_1: %d\n", *Mux_A.Input_1);
	//printf("Mux_A.Input_2: %d\n", *Mux_A.Input_2);
	//printf("Mux_A.Result: %d\n", Mux_A.Result);
	mux_B();
	//printf("Mux_B.Control: %d\n", *Mux_B.Control);
	//printf("Mux_B.Input_0: %d\n", *Mux_B.Input_0);
	//printf("Mux_B.Input_1: %d\n", *Mux_B.Input_1);
	//printf("Mux_B.Input_2: %d\n", *Mux_B.Input_2);
	//printf("Mux_B.Result: %d\n", Mux_B.Result);
	mux_4();
	//printf("Mux_4.Control%d\n", *Mux_4.Control);
	//printf("Mux_4.Input_0%d\n", *Mux_4.Input_0);
	//printf("Mux_4.Input_1%d\n", *Mux_4.Input_1);
	//printf("Mux_4.Result%d\n", Mux_4.Result);
	alu();
	and_gate();
	
	//step 2, ID step, the sequence of the components DOES CARE//
	instruction_partitioner();//although no such unit in the graph
	control();
	hazard_detection();
	//printf("Hazard_Detection_Unit.IDEX_MemRead: %d\n", *Hazard_Detection_Unit.IDEX_MemRead);
	//printf("Hazard_Detection_Unit.IDEX_Rd: %d\n", *Hazard_Detection_Unit.IDEX_Rd);
	//printf("Hazard_Detection_Unit.IFID_Rsrc1: %d\n", *Hazard_Detection_Unit.IFID_Rsrc1);
	//printf("Hazard_Detection_Unit.IFID_Rsrc2: %d\n", *Hazard_Detection_Unit.IFID_Rsrc2);
	//printf("Hazard_Detection_Unit.IFID_Hold: %d\n", Hazard_Detection_Unit.IFID_Hold);
	//printf("Hazard_Detection_Unit.PC_Hold: %d\n", Hazard_Detection_Unit.PC_Hold);
	//printf("Hazard_Detection_Unit.Stall: %d\n", Hazard_Detection_Unit.Stall);
	two_bits_shifter();
	address_adder();
	stall_mux();
	//printf("$s1 = %d\n", Reg[6]);
	registers_read();
	//printf("$s1 = %d\n", Reg[6]);
	//printf("Registers.Control_RegWrite: %d\n", *Registers.Control_RegWrite);
	//printf("Registers.Read_Register_1: %d\n", *Registers.Read_Register_1);
	//printf("Registers.Read_Register_2: %d\n", *Registers.Read_Register_2);
	//printf("Registers.Write_Data: %d\n", *Registers.Write_Data);
	//printf("Registers.Write_Register: %d\n", *Registers.Write_Register);
	//printf("Registers.Read_Data_1: %d\n", Registers.Read_Data_1);
	//printf("Registers.Read_Data_2: %d\n", Registers.Read_Data_2);
	
	if( Instruction_Partitioner.Opcode == 0x0f && (Instruction_Partitioner.Funct == 0X18 || Instruction_Partitioner.Funct == 0x1a) )
		//mult or div, the destination register is $lo, i.e 5
	{
		*Mux_3.Input_1 = 5;
	}
	mux_3();
	
	//step 1, IF step, the sequence of the component DOES CARE
	program_counter();// Example: the program counter is called at the very beginning part of a cycle
	instruction_memory();
	pc_adder();
	mux_1();
	mux_2();
	mux_PC();
	
	/********* Start - Do not touch **********/
	int endOrNot = 0;
	if (instruction_memory()) {
		endOrNot = 1;
	}
	/********* END - Do not touch ************/
	
	
	
	
	/********* Start - Do not touch **********/
	// Return 0 for not the end of the program
	// Return 1 for the end of the program
	return endOrNot;
	/********* END - Do not touch ************/
}

/********* Start - Do not touch **********/
void print_final_states(int cycle_count) {
	int i = 0;
	int j = 0;
	int reg_index;
	int reg_colume = 4;
	int reg_row = 32 / reg_colume;

	// Print contents in the components	
	printf("\n=======================================================================================================\n\n");
	printf(" Dumping Component States after Cycle [01;31m%3d[0m:\n", cycle_count);
	printf(" [01;35m[PC][0m          PC:              %04x\n",  Program_Counter.PC);
//	printf(" [01;35m[PC][0m          PC:              %04x\n",  Mux_3.Result);
	printf(" [01;35m[Instruction in stage 2][0m         Instruction:  [01;36m%08x[0m  \n", IFID.Output_Instruction);
	printf(" [01;35m[Instruction in stage 1][0m         Instruction:  [01;36m%08x[0m  \n", Instruction_Memory.Instruction);
	printf(" [01;35m[ALU][0m         ALU_Result:  [01;36m%08x[0m   Zero: [01;34m%04x[0m\n", ALU.ALU_Result, ALU.Zero);
	printf(" [01;35m[ALU Control][0m Result:          [01;34m%04x[0m\n", ALU_Control.Result);
	printf(" [01;35m[Control][0m     MemToReg:        [01;34m%04x[0m   MemRead:         [01;34m%04x[0m   MemWrite:     [01;34m%04x[0m   RegWrite:     [01;34m%04x[0m\n", Control.MemToReg, Control.MemRead, Control.MemWrite, Control.RegWrite);
	printf("               Jump:            [01;34m%04x[0m   JumpReg:         [01;34m%04x[0m   JumpLink:     [01;34m%04x[0m   Branch:       [01;34m%04x[0m\n", Control.Jump, Control.JumpReg, Control.JumpLink, Control.Branch);
	printf("               ALUSrc:          [01;34m%04x[0m   ALUOp:           [01;34m%04x[0m   RegSrc:       [01;34m%04x[0m   \n", Control.ALUSrc, Control.ALUOp, Control.RegSrc);
	//printf("               ALUSrc:          [01;34m%04x[0m   ALUOp:           [01;34m%04x[0m\n", Control.ALUSrc, Control.ALUOp);
	printf(" [01;35m[Registers][0m   Read_Data_1: [01;32m%08x[0m   Read_Data_2: [01;32m%08x[0m\n", Registers.Read_Data_1, Registers.Read_Data_2);
	printf(" [01;35m[Data Memory][0m Read_Data:   [01;32m%08x[0m\n", Data_Memory.Read_Data);
	printf(" [01;35m[Mux 1][0m       Result:          %04x\n", Mux_1.Result);
	printf(" [01;35m[Mux 2][0m       Result:          %04x\n", Mux_2.Result);
	printf(" [01;35m[Mux 3][0m       Result:          %04x\n", Mux_3.Result);
	printf(" [01;35m[Mux 4][0m       Result:      [01;32m%08x[0m\n", Mux_4.Result);
	printf(" [01;35m[Mux 5][0m       Result:      [01;32m%08x[0m\n", Mux_5.Result);
	//printf(" [01;35m[Mux 6][0m       Result:      [01;32m%08x[0m\n", Mux_6.Result);
	printf(" [01;35m[MuxPC][0m       Address:         %04x\n", Mux_PC.Final_Address);
	printf(" [01;35m[Mux A][0m       Result:      [01;32m%08x[0m\n", Mux_A.Result);
	printf(" [01;35m[Mux B][0m       Result:      [01;32m%08x[0m\n", Mux_B.Result);
	printf(" [01;35m[Funct][0m       Result:          %04x\n", Instruction_Partitioner.Funct);
	printf(" [01;35m[EX/MEM][0m      Output_ALUout:          %04x\n", EXMEM.Output_ALUout);
	printf(" [01;35m[MEM/WB][0m      Output_ALUout:          %04x\n", MEMWB.Output_ALUout);
	printf(" [01;35m[IDEX][0m        Output_Branch_Address:  [01;36m%04x[0m  \n", IDEX.Output_Branch_Address);
	// Print Contents in the Registers
	printf("\n Dumping Register Content:\n");
	for (i = 0; i < reg_row; i++) {
		for (j = 0; j < reg_colume; j++) {
			reg_index = i * reg_colume + j;
			printf(" [01;35m%s[0m%s%s  %08x    ", RegName[reg_index], strlen(RegName[reg_index]) < 4 ? " " : "", strlen(RegName[reg_index]) < 5 ? " " : "", Reg[reg_index]);
		}
		printf("\n");
	}

	// Print non-Zero Contents in the Memory
	printf("\n Dumping Memory Content (Non-Zero Contents):\n");
	for (i = 0; i < MEM_SIZE; i++) {
		if (Mem[i] != 0) {
			printf(" [01;35m%05x[0m  %08x\n", i*4, Mem[i]);
		}
	}

	printf("\n=======================================================================================================\n");
}

void simulate_Program() {
	
	int end_of_program;
	int cycle_count;
	//char terminate;

	end_of_program = 0;
	cycle_count = 0;

	// Loop the Instructions until the Program End
	while (!end_of_program) {
		cycle_count++;
		end_of_program = simulate_Cycle();
		print_final_states(cycle_count);
		// Print the Components states and Register / Memory content in each cycle
		if (!end_of_program) {
			/*
			printf("\n Press ENTER to continue...");
			do {
				scanf("%c", &terminate);
			} while (terminate != '\n');
			*/
			
		} else {
			//round it up with 4 cycles
			printf("\n Last 4 cycles \n");
			int i=0;
			for(i=0; i<4; i++){
				cycle_count++;
				end_of_program = simulate_Cycle();
				print_final_states(cycle_count);
				if(!end_of_program)break;
			}
			//printf("\n Program end\n");
		}
	}
}

int main(int argc, char **argv) {
	int i;

	// Check for the number of input argument(s)
	if (argc < 2) {
		printf("\nPlease input your machine code filename as the 1st arguement.\n");
		printf("> %s <machine_code>\n\n", argv[0]);
		return 0;
	}
	
	// Initialize the content in the memory and register
	memset(Mem, 0, MEM_SIZE * sizeof(unsigned));
	memset(Reg, 0, MAX_REG_SIZE * sizeof(unsigned));

	// Check and Read the Machine Code file
	i = read_machine_code_file(argv[1]);
	if (i == -1) {
		printf("\nPlease make sure you have typed the filename, [01;33m%s[0m, correctly.\n", argv[1]);
		printf("Or please check for the premission of the file.\n\n");
		return 0;
	} else if (i == 0) {
		printf("\nPlease make sure the input file, [01;33m%s[0m, is not empty.\n\n", argv[1]);
	}

	// Connect the Components according to the datapath in the specification
	connect_components();
	
	// Initialize the registers and states in components
	initialize_register_states();
	
	// Simulate the instruction in the Memory
	simulate_Program();
	
	return 0;
}
/********* END - Do not touch ************/
