/*
 * CSC3420 Project Phase 2
 * Single-cycle simulator
 *
 * This is the file implementing the component methods
 * Please complete the component methods according to the project specification.
 *
 * !!! Do NOT declare any global variables and new functions in this file !!!
 * !!! Control Signal is stored in char datatype, it allows ONLY ONE bit to be passed !!!
 *
 */

#include "components.h"

// Definition of component methods for Multiple-Division Unit
void run_mult_div(unsigned AData1, unsigned AData2, unsigned AF, unsigned *Result, unsigned *Result64);
void run_register32(unsigned In, unsigned *Content);
void run_sub_alu(unsigned D1, unsigned D2, unsigned ToDo, unsigned *RST);
void run_shifter64(unsigned In, unsigned InAcc, char ShiftRight, char ShiftLeft, char Write, unsigned *Left, unsigned *Right);
void run_test_control(unsigned Left, unsigned Right, unsigned AF, char *ShiftRight, char *ShiftLeft, char *Write, unsigned *ToDo);

// Example
// This method simulates how Program_Counter is running
// The next PC values to copy to the output terminal of Program_Counter
void run_program_counter(unsigned Next, unsigned *PC) {
	*PC = Next;
}

// Complete the following functions

// This methods reads the content in the binary file (file descripter: input_file)
// And store the read instruction (1 word size) to the memory (start at Mem[0])
// Please note that rge machine is big-endian
void read_machine_code_file(FILE *input_file) {
	// Fill in your code here
	
	//get the binary file size
	unsigned file_size;
	fseek(input_file,0,SEEK_END);
	file_size = ftell(input_file);
	
	//read the binary file into buffer
	rewind(input_file);
	char* buffer = malloc(file_size + 1);
	fread(buffer, file_size, 1, input_file);
	
	//change into big-endian(every four bytes) and put them into Men[]
	unsigned file_count = 0;
	int i = 0;//count Mem[]
	int j = 0;//count buffer
	while(file_count < file_size)
	{
		char temp[4];
		int k = 3;//count temp
		for( k = 3; k >= 0; k--, j++)
		{
			temp[k] = buffer[j];
		}
		Mem[i] = ((temp[3] << 24)&0xff000000) | ((temp[2] << 16)&0x00ff0000) | ((temp[1] << 8)&0x0000ff00) | ((temp[0] << 0)&0x000000ff);
		file_count += 4;
		i++;
	}
	
	free(buffer);
}

// This method simulates how PC_Adder is running
// PC_Adder returns the sum of two input terminals
void run_pc_adder(unsigned D1, unsigned D2, unsigned *RST) {
	// Fill in your code here
	*RST = D1 + D2;
}

// This method simulates how Instruction_Memory is running
// Instruction_Memory returns the particular instruction in address: Address
void run_instruction_memory(unsigned Address, unsigned *Instruction) {
	// Fill in your code here
	*Instruction = Mem[Address>>2];//word address to byte address
}

// This method simulates how Instruction_Partitioner is running
// Instruction_Partitioner decodes the instruction and partition the word-size instruction into its output terminals
void run_instruction_partitioner(unsigned Instruction, unsigned *Opcode, unsigned *Rs, unsigned *Rt, unsigned *Rd, unsigned *Offset_imm, unsigned *Target, unsigned *Funct) {
	// Fill in your code here
	*Opcode = (Instruction >> 26) & 0x0000003f;
	*Rs = ((Instruction << 6) >> 27) & 0x0000001f;
	*Rt = ((Instruction << 11) >> 27) & 0x0000001f;
	*Rd = ((Instruction << 16) >> 27) & 0x0000001f;
	*Offset_imm = ((Instruction << 16) >> 16) & 0x0000ffff;
	*Target = ((Instruction << 6) >> 6) & 0x03ffffff;
	*Funct = ((Instruction << 26) >> 26) & 0x0000003f;
}

// This method simulates how Control unit is running
// Control unit outputs control signals to drive other conponents to complete the current instruction depends on Opcode
// Please note that control signals (char datatype) are allowed to carry ONLY ONE bit signal (either 0 or 1)
int run_control(unsigned Opcode, char *MemWrite, char *Branch, char *Jump, char *MemToReg, char *ALUSrc, char *ALUOp, char *RegWrite, char *IType, char *JumpLink) {
	// Fill in your code here
	if(Opcode == 1)//addi
	{
		*MemWrite = 0x00;
		*Branch = 0x00;
		*Jump = 0x00;
		*MemToReg = 0x00;
		*ALUSrc = 0x01;
		*ALUOp = 0x00;//0x01
		*RegWrite = 0x01;
		*IType = 0x01;
		*JumpLink = 0x00;
		return 1;
	}
	else if(Opcode == 0x1a)//lw
	{
		*MemWrite = 0x00;
		*Branch = 0x00;
		*Jump = 0x00;
		*MemToReg = 0x01;
		*ALUSrc = 0x01;
		*ALUOp = 0x00;//0x01
		*RegWrite = 0x01;
		*IType = 0x01;//0x00
		*JumpLink = 0x00;
		return 1;
	}
	else if(Opcode == 0x1b)//sw
	{
		*MemWrite = 0x01;
		*Branch = 0x00;
		*Jump = 0x00;
		*MemToReg = 0x00;
		*ALUSrc = 0x01;
		*ALUOp = 0x00;//0x01
		*RegWrite = 0x00;
		*IType = 0x00;
		*JumpLink = 0x00;
		return 1;
	}
	else if(Opcode == 2)//bne
	{
		*MemWrite = 0x00;
		*Branch = 0x01;
		*Jump = 0x00;
		*MemToReg = 0x00;
		*ALUSrc = 0x00;
		*ALUOp = 0x00;//0x01
		*RegWrite = 0x00;
		*IType = 0x00;
		*JumpLink = 0x00;
		return 1;
	}
	else if(Opcode == 3)//j
	{
		*MemWrite = 0x00;
		*Branch = 0x00;
		*Jump = 0x01;
		*MemToReg = 0x00;
		*ALUSrc = 0x0;
		*ALUOp = 0x00;
		*RegWrite = 0x00;
		*IType = 0x00;
		*JumpLink = 0x00;
		return 1;
	}
	else if(Opcode == 4)//jal
	{
		*MemWrite = 0x00;
		*Branch = 0x00;
		*Jump = 0x01;
		*MemToReg = 0x00;
		*ALUSrc = 0x00;
		*ALUOp = 0x00;
		*RegWrite = 0x01;
		*IType = 0x00;
		*JumpLink = 0x01;
		return 1;
	}
	else if(Opcode == 0x0f)//R-tpe
	{
		*MemWrite = 0x00;
		*Branch = 0x00;
		*Jump = 0x00;
		*MemToReg = 0x00;
		*ALUSrc = 0x00;
		*ALUOp = 0x01;
		*RegWrite = 0x01;
		*IType = 0x00;
		*JumpLink = 0x00;
		return 1;
	}
	else
	{
		return 0;
	}
}

// This method simulates how Register is read
// The register outputs the register contents based on the register index
void run_register_read(unsigned Rs, unsigned Rt, unsigned Rd, unsigned *RsData, unsigned *RtData) {
	// Fill in your code here
	*RsData = Reg[Rs];
	*RtData = Reg[Rt];
}

// This method simulates how register is being written
// The register updated the content in the register unit depends on the register index and control signals
void run_register_write(unsigned Rs, unsigned Rt, unsigned Rd, unsigned WData, unsigned WData64, char JumpLink, char Data64, char IType, char RegWrite) {
	// Fill in your code here
	if(JumpLink == 0x01 && Data64 == 0x00 && IType == 0x00 && RegWrite == 0x01)//jal
	{
		Reg[1] = WData;
	}
	else if(JumpLink == 0x00 && Data64 == 0x00 && IType == 0x01 && RegWrite == 0x01)//addi and lw
	{
		Reg[Rt] = WData;
	}
	else if(JumpLink == 0x00 && IType == 0x00 && RegWrite == 0x01)// 0x0f instruction type
	{
		if(Data64 == 0x00)
		{
			Reg[Rd] = WData;
		}
		else if(Data64 == 0x01)//div and mult
		{
			Reg[4] = WData64;//Hi
			Reg[5] = WData;//Lo
		}
	}
	
	Reg[0] = 0;//make sure reg_zero is always 0
}

// This method simulates how ALU_Control is running
// The ALU_Control outputs function code to ALU
int run_alu_control(unsigned Funct, char ALUOp, unsigned *AF, char *Data64, char *JumpReg) {
	// Fill in your code here
	if(ALUOp == 0x01)
	{
		if(Funct == 0x11)//add AF = 0000
		{
			*AF = 0;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x12)//sub AF = 0001
		{
			*AF = 1;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x18)//mult AF = 0010//0x18
		{
			*AF = 24;
			*Data64 = 0x01;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x1a)//div AF = 0011//0x1a
		{
			*AF = 26;
			*Data64 = 0x01;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x2a)//and AF = 0100
		{
			*AF = 4;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x2b)//or AF = 0101
		{
			*AF = 5;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x01)//sllv AF = 0110
		{
			*AF = 6;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x02)//srlv AF = 0111
		{
			*AF = 7;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x25)//slt AF = 1000
		{
			*AF = 8;
			*Data64 = 0x00;
			*JumpReg = 0x00;
		}
		else if(Funct == 0x03)//jra AF = 1001
		{
			*AF = 9;
			*Data64 = 0x00;
			*JumpReg = 0x01;
		}
		else
		{
			return 0;
		}
	}
	
	else if(ALUOp == 0)//SW or LW or ADDI or BNE. no J and JAL. AF = 1010
	{
		*AF = 10;
		*Data64 = 0x00;
		*JumpReg = 0x00;
	}
	
	return 1;
}

// This method simulates how ALU is running
// ALU computes the result based on the 2 inputs and the function code
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 	ALU calls run_mult_div() to simulate multiplication and division
//  You are not allowed use to A * B or A / B or A % B
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
int run_alu(unsigned AData1, unsigned AData2, unsigned AF, char *Zero, unsigned *Result, unsigned *Result64) {
	// Fill in your code here
	if(AF > 26)
	{
		return 0;
	}
	else
	{
		switch(AF)
		{
			case 0://add, note that signed bit extend
			{
				/*unsigned temp1 = AData1 & 0x7fffffff;
				unsigned temp2 = AData2 & 0x7fffffff;
				
				if( (((AData1 >> 31) + (AData2 >> 31)) & 0x00000001) == 0 )//signed bit of AData1 and AData2 is (0,0) or (1,1)
				{
					*Result = temp1 + temp2;
				}
				else if( (((AData1 >> 31) + (AData2 >> 31)) & 0x00000001) == 1 )//signed bit of AData1 and AData2 is (0,1) or (1,0)
				{
					if( ((temp1 + temp2) >> 31) == 1 )
					{
						*Result = (temp1 + temp2) & 0x7fffffff;
					}
					else if( ((temp1 + temp2) >> 31) == 0 )
					{
						*Result = AData1 + AData2;
					}
				}*/
				//an easier way
				int64_t temp = AData1 + AData2;
				*Result = temp & 0x00000000ffffffff;
			}break;
			case 1://sub, note that unsigned type will have segmentation fault when result is minus number
			{
				//*Result = AData1 - AData2;
				int64_t temp = AData1 - AData2;
				*Result = temp & 0x00000000ffffffff;
			}break;
			case 24://mult,0x18
			{
				run_mult_div(AData1, AData2, 24, Result, Result64);
			}break;
			case 26://div, 0x1a
			{
				run_mult_div(AData1, AData2, 26, Result, Result64);
			}break;
			case 4://and
			{
				*Result = AData1 & AData2;
			}break;
			case 5://or
			{
				*Result = AData1 | AData2;
			}break;
			case 6://sllv
			{
				*Result = AData2 << AData1;
			}break;
			case 7://srlv
			{
				*Result = AData2 >> AData1;
			}break;
			case 8://slt
			{
				if( (AData1 >> 31) == 0 && (AData2 >> 31) == 1 )//positive : minus
				{
					*Result = 0;
				}
				else if( (AData1 >> 31) == 1 && (AData2 >> 31) == 0 )//minus : positive
				{
					*Result = 1;
				}
				else if( (AData1 >> 31) == 0 && (AData2 >> 31) == 0 )//positive : positive
				{
					*Result = (AData1 < AData2? 1 : 0);
				}
				else if( (AData1 >> 31) == 1 && (AData2 >> 31) == 1 )//minus : minus
				{
					unsigned temp1 = AData1 & 0x7fffffff;
					unsigned temp2 = AData2 & 0x7fffffff;
					
					*Result = (temp1 < temp2? 1 : 0);
				}
			}break;
			case 9://jra
			{
				*Result = AData2;//no used
			}break;
			case 10://sw, lw, addi, bne
			{
				/*unsigned temp1 = AData1 & 0x7fffffff;
				unsigned temp2 = AData2 & 0x7fffffff;
				
				if( (((AData1 >> 31) + (AData2 >> 31)) & 0x00000001) == 0 )//signed bit of AData1 and AData2 is (0,0) or (1,1)
				{
					*Result = temp1 + temp2;
				}
				else if( (((AData1 >> 31) + (AData2 >> 31)) & 0x00000001) == 1 )//signed bit of AData1 and AData2 is (0,1) or (1,0)
				{
					if( ((temp1 + temp2) >> 31) == 1 )
					{
						*Result = (temp1 + temp2) & 0x7fffffff;
					}
					else if( ((temp1 + temp2) >> 31) == 0 )
					{
						*Result = AData1 + AData2;
					}
				}*/
				//an easier way
				int64_t temp = AData1 + AData2;
				*Result = temp & 0x00000000ffffffff;
				*Zero = (AData1 == AData2? 0x00 : 0x01);
			}break;
		}
	}
	
	return 1;
}

// This method simulates how AND_Gate is running
// AND_Gate outputs the condition AND of 2 input terminals
void run_and_gate(char D1, char D2, char *Output) {
	// Fill in your code here
	*Output = D1 & D2;
}

// This method simulates how OR_Gate is running
// OR_Gate outputs the condition OR of 2 input terminals
void run_or_gate(char D1, char D2, char *Output) {
	// Fill in your code here
	*Output = D1 | D2;
}

// This method simulates how Sign_Extend is running
// Sign_Extend extend the sign bits
void run_sign_extend(unsigned In, unsigned *Out) {
	// Fill in your code here
	unsigned temp = In & 0x00008000;
	if(temp == 0x00008000)//minus number
	{
		*Out = 0xffff0000 | In;
	}
	else
	{
		*Out = In;
	}
}

// This method simulates how Two-bit_Shifter is running
void run_twobit_shifter(unsigned In, unsigned *Out) {
	// Fill in your code here
	*Out = In << 2;
}

// This method simulates how Address_Adder is running
// Address_Adder returns the sum of two input terminals
void run_address_adder(unsigned D1, unsigned D2, unsigned *RST) {
	// Fill in your code here
	*RST = D1 + D2;
}

// This method simulates how Memory is read and write
// Memory can either return the content or update the content in adreess: Address depends on the control signal
void run_data_memory(unsigned Address, unsigned WData, char MemWrite, unsigned *RData) {
	// Fill in your code here
	if(MemWrite == 0x01)//update the content in address
	{
		Mem[Address>>2] = WData;
	}
	else if(MemWrite == 0x00)//return the content in address
	{
		if( (Address >> 2) <= MEM_SIZE)
		{ 
			*RData = Mem[Address>>2];
		}
	}
}

// This method simulates how Mux is running
// Mux returns Data0 if the control is 0 or returns Data1 if the control is 1
void run_mux(unsigned D0, unsigned D1, char Control, unsigned *RST) {
	// Fill in your code here
	*RST = (Control == 0x00? D0 : D1);
}

// This method simulates how Multiple-Division unit (MDU) is running
// You have to design the calling sequent of the MDU in order to drive the MDU conponents to perform multiplication and division
// Part of this function is implemeted for you, you have to fill in the calling sequent
void run_mult_div(unsigned AData1, unsigned AData2, unsigned AF, unsigned *Result, unsigned *Result64) {
	
	// Declare the internal hardware components in the Mult_Div unit
	Register32 register32;
	Sub_ALU sub_alu;
	Shifter64 shifter64;
	Test_Control test_control;

	// Connect the internal hardwares compoents together
	register32.In = AData2;
	sub_alu.D1 = &(shifter64.Left);
	sub_alu.D2 = &(register32.Content);
	sub_alu.ToDo = &(test_control.ToDo);
	shifter64.In = AData1;
	shifter64.ShiftRight = &(test_control.ShiftRight);
	shifter64.ShiftLeft = &(test_control.ShiftLeft);
	shifter64.Write = &(test_control.Write);
	shifter64.InAcc = &(sub_alu.RST);
	shifter64.Right = shifter64.In;
	if (shifter64.Right >> 31) {
		shifter64.Left = 0xFFFFFFFF;
	} else {
		shifter64.Left = 0;
	}
	test_control.Left = &(shifter64.Left);
	test_control.Right = &(shifter64.Right);
	test_control.AF = AF;

	// Please fill you code here to simulate the calling sequent of MDU components
	if(AF == 24)//multiply
	{	
		run_register32(register32.In, &register32.Content);
		int i = 0;
		while(i < 32)
		{
			run_test_control(*(test_control.Left), *(test_control.Right), AF, 
								&test_control.ShiftRight, &test_control.ShiftLeft, &test_control.Write, &test_control.ToDo);
			run_sub_alu(*(sub_alu.D1), *(sub_alu.D2), *(sub_alu.ToDo), &sub_alu.RST);
			run_shifter64(shifter64.In, *(shifter64.InAcc), *(shifter64.ShiftRight), *(shifter64.ShiftLeft),
							*(shifter64.Write), &shifter64.Left, &shifter64.Right);
			i++;
		}
	}
	else if(AF == 26)//divide
	{
		/*shifter64.Left = AData1 % AData2;
		shifter64.Right = AData1 / AData2;*/
		//printf("%x", (shifter64.Right));
		run_register32(register32.In, &register32.Content);
		//first need to shift left 1 bit
		//printf("\n");
		run_test_control(*(test_control.Left), *(test_control.Right), AF, 
								&test_control.ShiftRight, &test_control.ShiftLeft, &test_control.Write, &test_control.ToDo);
		//just shift left 1 bit
		int64_t temp_l = shifter64.Left;
		int64_t temp_r = shifter64.Right;
		temp_l = (temp_l << 32) & 0xffffffff00000000;
		temp_r = temp_r & 0x00000000ffffffff;
		int64_t result = temp_l | temp_r;
		result = result << 1;
		shifter64.Left = result >> 32;
		shifter64.Right = result & 0x00000000ffffffff;
		
		int i = 0;
		while(i < 32)
		{
			//printf("\n");
			run_sub_alu(*(sub_alu.D1), *(sub_alu.D2), *(sub_alu.ToDo), &sub_alu.RST);
			run_shifter64(shifter64.In, *(shifter64.InAcc), *(shifter64.ShiftRight), *(shifter64.ShiftLeft),
							1, &shifter64.Left, &shifter64.Right);//No shift, just write
			run_test_control(*(test_control.Left), *(test_control.Right), AF, 
								&test_control.ShiftRight, &test_control.ShiftLeft, &test_control.Write, &test_control.ToDo);
			run_shifter64(shifter64.In, *(shifter64.InAcc), *(shifter64.ShiftRight), *(shifter64.ShiftLeft),
							*(shifter64.Write), &shifter64.Left, &shifter64.Right);//No write, just shift
			i++;
		}
		//shift back, if the remainder(HI) is positive, just shift right 1 bit, else, shift right 1 bit and add back the divider
		if( (shifter64.Left & 0x80000000) != 0 )//left most bit is one, negative remainder
		{
			shifter64.Left = (shifter64.Left >> 1) | 0x80000000;
			shifter64.Left = shifter64.Left + register32.Content;
		}
		else//left most bit is 0, positive remainder
		{
			shifter64.Left = shifter64.Left >> 1;
		}
	}
	


	// Result should be return from the shifter_64.
	*Result = shifter64.Right;
	*Result64 = shifter64.Left;
}

// This method simulates how Register_32 in MDU is running
void run_register32(unsigned In, unsigned *Content) {
	// Fill in your code here
	*Content = In;
}

// This method simulates how Sub_ALU in MDU is running
void run_sub_alu(unsigned D1, unsigned D2, unsigned ToDo, unsigned *RST) {
	// Fill in your code here
	if(ToDo == 0)//add
	{
		int64_t temp1 = D1 + D2;
		*RST = temp1 & 0x00000000ffffffff;
	}
	else if(ToDo == 1)//sub
	{
		int64_t temp2 = D1 - D2;
		*RST = temp2 & 0x00000000ffffffff;
	}
}

// This method simulates how Shifter_64 in MDU is running
// Shifter_64 updated the content of its Right and Left register depends on the control signals
void run_shifter64(unsigned In, unsigned InAcc, char ShiftRight, char ShiftLeft, char Write, unsigned *Left, unsigned *Right) {
	// Fill in your code here
	if(1)//In >= 0)//positive, always positive
	{
		if(ShiftRight == 0x01 && ShiftLeft == 0x00)//mult
		{
			if(Write == 0x00)//don't write, just shift
			{
				int64_t temp_l = *Left;
				int64_t temp_r = *Right;
				temp_l = (temp_l << 32) & 0xffffffff00000000;
				temp_r = temp_r & 0x00000000ffffffff;
				int64_t result = temp_l | temp_r;
				result = result >> 1;
				*Left = result >> 32;
				*Right = result & 0x00000000ffffffff;
			}
			else if(Write == 0x01)//write
			{
				int64_t temp_l = InAcc;
				int64_t temp_r = *Right;
				temp_l = (temp_l << 32) & 0xffffffff00000000;
				temp_r = temp_r & 0x00000000ffffffff;
				int64_t result = temp_l | temp_r;
				result = result >> 1;
				*Left = (result >> 32);
				*Right = result & 0x00000000ffffffff;
			}
		}
		else if(ShiftRight == 0x00 && ShiftLeft == 0x01)//div
		{
			//printf("Shifter64:\n");
			if(Write == 0x01)//No Shift, just Write, hence the run_test_control know the result of the ALU is positive or negative
			{
				*Left = InAcc;
			}
			else if(Write == 0x00)//No Write, just shift, right most bit is depend on the InACC
			{
				int64_t temp_l = *Left;
				int64_t temp_r = *Right;
				temp_l = (temp_l << 32) & 0xffffffff00000000;
				temp_r = temp_r & 0x00000000ffffffff;
				int64_t result = temp_l | temp_r;
				result = result << 1;//shift left
				if( ((*Left) & 0x80000000) ==  0)//positive ALU result, right most bit is 1;
				{
					result = result | 0x0000000000000001;
				}
				*Left = result >> 32;
				*Right = result & 0x00000000ffffffff;
			}
			//printf("In = %X, InAcc = %x, ShiftLeft = %x, ShiftRight = %x, Write = %x\n", In, InAcc, ShiftLeft, ShiftRight, Write);
			//printf("Left = %x, Right = %x\n", *Left, *Right);
		}
	}
	//else if(In < 0)//negative no need since we assume all are positive number
	//{
	//}
}

// This method simulates how Test_Control in MDU is running
// Test_Control generate control signals to Shifter_64 and Sub_ALU for performing multiplication and division corrently
void run_test_control(unsigned Left, unsigned Right, unsigned AF, char *ShiftRight, char *ShiftLeft, char *Write, unsigned *ToDo) {
	// Fill in your code here
	if(AF == 0x18)//multiply
	{
		if((Right & 0x00000001) == 1)//right most bit is 1
		{
			*ShiftRight = 0x01;//shift right
			*ShiftLeft = 0x00;//don't shift left
			*Write = 0x01;//need to write into the 64-shifter
			*ToDo = 0;//add
		}
		else if((Right & 0x00000001) == 0)//right most bit is 0
		{
			*ShiftRight = 0x01;//shift right
			*ShiftLeft = 0x00;//don't shift left
			*Write = 0x00;//no need to write into the 64-shifter
			*ToDo = 0;//add
		}
	}
	else if(AF == 0x1a)//divide
	{
		if((Left & 0x80000000) == 0x80000000)//negative remainder, non-restore,next ALU operation is add
		{
			*ShiftRight = 0x00;//don't shift right
			*ShiftLeft = 0x01;//shift left
			*Write = 0x00;//don't write, just shift
			//*Write = 0x01;//need to write into the 64-shifter
			*ToDo = 0;//add
		}
		else if((Left & 0x80000000) == 0)//positive remainder, non-restore, next ALU operation is sub
		{
			*ShiftRight = 0x00;//don't shift right
			*ShiftLeft = 0x01;//shift left
			*Write = 0x00;//don't write, just shift
			//*Write = 0x01;//need to write into the 64-shifter
			*ToDo = 1;//sub
		}
		//printf("Test_control:\n");
		//printf("ShiftLeft = %x, ShiftRight = %x, Write = %x, ToDo = %x\n", *ShiftLeft, *ShiftRight, *Write, *ToDo);
	}
}

// This method requires students to store their student ID to Register $s26
void run_store_sid() {
	// Fill in your code here
	Reg[31] = 1155026053;
}
