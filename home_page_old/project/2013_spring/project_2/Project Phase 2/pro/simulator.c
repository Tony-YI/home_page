/*
 * CSC3420 Project Phase 2
 * Single-cycle simulator
 *
 * This is the main program that initializes the components connection and calling sequent.
 *
 * !!! Do NOT modify this file, you have to submit phase2.c ONLY. !!!
 *
 */

#include "phase2.h"
#include "components.h"

// Define the Synbols for the Registers (for printing only)
const char RegName[MAX_REG_SIZE][6] = {
	"$zero", "$ra",  "$fp",  "$sp",  "$hi",  "$lo",  "$s1",  "$s2",
	"$s3",   "$s4",  "$s5",  "$s6",  "$s7",  "$s8",  "$s9",  "$s10",
	"$s11",  "$s12", "$s13", "$s14", "$s15", "$s16", "$s17", "$s18",
	"$s19",  "$s20", "$s21", "$s22", "$s23", "$s24", "$s25", "$s26"};

// Declare the Components
Program_Counter program_counter;
PC_Adder pc_adder;
Instruction_Memory instruction_memory;
Instruction_Partitioner instruction_partitioner;
Control control;
Register registers;
ALU_Control alu_control;
ALU alu;
AND_Gate and_gate;
OR_Gate or_gate;
Data_Memory data_memory;
Sign_Extend sign_extend;
Twobit_Shifter twobit_shifter;
Address_Adder address_adder;
Mux mux1;
Mux mux2;
Mux mux3;
Mux mux4;
Mux mux5;
Mux mux6;

void connect_components();
void initialization();
void print_content(int cycle);
void safe_free(void *memory);

// Main Function
int main(int argc, char **argv) {
	int cycle;
	FILE *input_file;

	// Control the components together with Data Bus and Control Lines
	connect_components();
	// Initialize the first PC value (first instruction address)
	// Set all regsiter and memory contents to ZERO
	initialization();

	// Open the File Pointer for the Machine Code file and Check for its exist
	// Return -1 if the file is not exist or not accessable
	// Close the file descriptor after reading the Machine Code file
	input_file = fopen(argv[1], "rb");
	if (input_file == NULL) {
		printf("Fail to read the Machine Code file.\nPlease read the Usgae Carefully!!\n\n");
		printf("Usage: %s machine_code.asc\n", argv[0]);
		printf("\n  where machine_code.asc is the assembled program (in binary).\n\n");
		return -1;
	}
	read_machine_code_file(input_file);
	fclose(input_file);

	printf("\n================================================================================\n\n");
	// The first cycle is numbered as 1
	cycle = 1;

	// Inside the while loop, the calling sequent of the components is implemented
	// The while loop will break when the fetched instruction is 0
	// After each cycle, the memory and register content will be dumped
	while (1) {
		// Instruction Fetch
		run_program_counter(*(program_counter.Next), &(program_counter.PC));
		run_pc_adder(pc_adder.D1, *(pc_adder.D2), &(pc_adder.RST));
		run_instruction_memory(*(instruction_memory.Address), &(instruction_memory.Instruction));
		// if the fetched instruction is 0, the while loop is terminated
		if (!instruction_memory.Instruction) {
			printf("\n1111111111111111111111111111111111111111111111111111111111\n");
			break;
		}
		// Instruction Decode
		run_instruction_partitioner(*(instruction_partitioner.Instruction), &(instruction_partitioner.Opcode), &(instruction_partitioner.Rs), &(instruction_partitioner.Rt), &(instruction_partitioner.Rd), &(instruction_partitioner.Offset_imm), &(instruction_partitioner.Target), &(instruction_partitioner.Funct));
		
		// if the fetched instruction is invalid, output stduent id and the while loop is terminated
		if (!run_control(*(control.Opcode), &(control.MemWrite), &(control.Branch), &(control.Jump), &(control.MemToReg), &(control.ALUSrc), &(control.ALUOp), &(control.RegWrite), &(control.IType), &(control.JumpLink)))
		{
			//store the sid
			run_store_sid();
			// print the register and memory content
			printf("\n2222222222222222222222222222222222222222222222222222222222\n");
			print_content(cycle);		
			break;
		}
		
		run_register_read(*(registers.Rs), *(registers.Rt), *(registers.Rd), &(registers.RsData), &(registers.RtData));
		if (!run_alu_control(*(alu_control.Funct), *(alu_control.ALUOp), &(alu_control.AF), &(alu_control.Data64), &(alu_control.JumpReg)))
		{
			//store the sid
			run_store_sid();
			// print the register and memory content
			printf("\n33333333333333333333333333333333333333333333333333333333333\n");
			print_content(cycle);
			break;
		}
		run_sign_extend(*(sign_extend.In), &(sign_extend.Out));
		run_mux(*(mux2.D0), *(mux2.D1), *(mux2.Control), &(mux2.RST));
		
		// Execution
		if (!run_alu(*(alu.AData1), *(alu.AData2), *(alu.AF), &(alu.Zero), &(alu.Result), &(alu.Result64)))
		{
			//store the sid
			run_store_sid();
			// print the register and memory content
			printf("\n4444444444444444444444444444444444444444444444444444444444444\n");
			print_content(cycle);
			break;
		}
		// Memory
		run_data_memory(*(data_memory.Address), *(data_memory.WData), *(data_memory.MemWrite), &(data_memory.RData));
		run_and_gate(*(and_gate.D1), *(and_gate.D2), &(and_gate.Output));
		run_twobit_shifter(*(twobit_shifter.In), &(twobit_shifter.Out));
		run_address_adder(*(address_adder.D1), *(address_adder.D2), &(address_adder.RST));
		run_mux(*(mux3.D0), *(mux3.D1), *(mux3.Control), &(mux3.RST));
		run_mux(*(mux4.D0), *(mux4.D1), *(mux4.Control), &(mux4.RST));
		run_mux(*(mux6.D0), *(mux6.D1), *(mux6.Control), &(mux6.RST));
		run_mux(*(mux1.D0), *(mux1.D1), *(mux1.Control), &(mux1.RST));
		
		// Write Back
		run_register_write(*(registers.Rs), *(registers.Rt), *(registers.Rd), *(registers.WData), *(registers.WData64), *(registers.JumpLink), *(registers.Data64), *(registers.IType), *(registers.RegWrite));
		run_or_gate(*(or_gate.D1), *(or_gate.D2), &(or_gate.Output));
		run_mux(*(mux5.D0), *(mux5.D1), *(mux5.Control), &(mux5.RST));
		
		// print the register and memory content
		print_content(cycle);
		cycle++;
	}

	return 0;
}

// Connect the components
void connect_components() {
	program_counter.Next = &(mux5.RST);
	pc_adder.D1 = WORD_SIZE;
	pc_adder.D2 = &(program_counter.PC);
	instruction_memory.Address = &(program_counter.PC);
	instruction_partitioner.Instruction = &(instruction_memory.Instruction);
	control.Opcode = &(instruction_partitioner.Opcode);
	registers.Rs = &(instruction_partitioner.Rs);
	registers.Rt = &(instruction_partitioner.Rt);
	registers.Rd = &(instruction_partitioner.Rd);
	registers.WData = &(mux1.RST);
	registers.WData64 = &(alu.Result64);
	registers.JumpLink = &(control.JumpLink);
	registers.Data64 = &(alu_control.Data64);
	registers.IType = &(control.IType);
	registers.RegWrite = &(control.RegWrite);
	mux1.D0 = &(mux6.RST);
	mux1.D1 = &(pc_adder.RST);
	mux1.Control = &(control.JumpLink);
	alu_control.Funct = &(instruction_partitioner.Funct);
	alu_control.ALUOp = &(control.ALUOp);
	mux2.D0 = &(registers.RtData);
	mux2.D1 = &(sign_extend.Out);
	mux2.Control = &(control.ALUSrc);
	alu.AData1 = &(registers.RsData);
	alu.AData2 = &(mux2.RST);
	alu.AF = &(alu_control.AF);
	and_gate.D1 = &(control.Branch);
	and_gate.D2 = &(alu.Zero);
	or_gate.D1 = &(control.Jump);
	or_gate.D2 = &(alu_control.JumpReg);
	data_memory.Address = &(alu.Result);
	data_memory.WData = &(registers.RtData);
	data_memory.MemWrite = &(control.MemWrite);
	sign_extend.In = &(instruction_partitioner.Offset_imm);
	twobit_shifter.In = &(sign_extend.Out);
	address_adder.D1 = &(pc_adder.RST);
	address_adder.D2 = &(twobit_shifter.Out);
	mux3.D0 = &(pc_adder.RST);
	mux3.D1 = &(address_adder.RST);
	mux3.Control = &(and_gate.Output);
	mux4.D0 = &(instruction_partitioner.Target);
	mux4.D1 = &(registers.RsData);
	mux4.Control = &(alu_control.JumpReg);
	mux5.D0 = &(mux3.RST);
	mux5.D1 = &(mux4.RST);
	mux5.Control = &(or_gate.Output);
	mux6.D0 = &(alu.Result);
	mux6.D1 = &(data_memory.RData);
	mux6.Control = &(control.MemToReg);
}

// initialize the first PC value
// Set all the register contents and memory contents to ZERO
void initialization() {
	int i;

	*(program_counter.Next) = PCINIT;
	for (i = 0; i < MAX_REG_SIZE; i++)
		Reg[i] = 0;
	for (i = 0; i < MEM_SIZE; i++)
		Mem[i] = 0;
}

// Free the memory if the memory pointer is not NULL
void safe_free(void *memory) {
	if (memory != NULL) {
		free(memory);
	}
}

// Dump the content of each register and non-empty memory
void print_content(int cycle) {
	int i, j, reg_index;
	
	// Print the number of cycle
	printf(" After Cycle %3d:\n", cycle);
	printf(" ================\n");
	
	// Print Contents in the Registers
	printf("\n Dumping Register Content:\n");
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 4; j++) {
			reg_index = i * 4 + j;
			printf(" %s%s%s  %08x    ", RegName[reg_index], strlen(RegName[reg_index]) < 4 ? " " : "", strlen(RegName[reg_index]) < 5 ? " " : "", Reg[reg_index]);	
		}
		printf("\n");
	}
	
	// Print non-Zero Contents in the Memory
	printf("\n Dumping Memory Content (Non-Zero Contents):\n");
	for (i = 0; i < MEM_SIZE; i++) {
		if (Mem[i] != 0) {
			printf(" %05x  %08x\n", i*4, Mem[i]);
		}
	}
	printf("\n================================================================================\n\n");
}
