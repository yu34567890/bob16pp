/*
ignore some parts theyre just there because for me to look up and steal some code
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef int16_t reg_t;

typedef enum INSTRUCTION_E {
	NOP,
	ADD,
	AND,
	NOT,
	LD,
	LDI,
	LDR,
	ST,
	STI,
	STR,
	BR,
	JMP,
	JSR,
	LEA,
	RET,
	TRAP
} INSTRUCTION;

typedef enum TRAP_VECTOR_E {
	HALT,
	PUTC,
	PUTS,
	GETS,
	EXTEND
} TRAP_VECTOR;


#define sext_4(x)  (((x) >= 0x8)   ? (int16_t)((x) | 0xFFF0) : (x))
#define sext_6(x)  (((x) >= 0x20)  ? (int16_t)((x) | 0xFFC0) : (x))
#define sext_7(x)  (((x) >= 0x40)  ? (int16_t)((x) | 0xFF80) : (x))
#define sext_9(x)  (((x) >= 0x100) ? (int16_t)((x) | 0xFE00) : (x))
#define sext_11(x) (((x) >= 0x400) ? (int16_t)((x) | 0xF800) : (x))

reg_t    registers[8]; 
uint16_t memory[65535];
uint16_t pc;
uint8_t  flag_register;
uint16_t current_instruction;
uint16_t memory_adress_register; // why does this exist

void updateCC(int16_t val)
{
    flag_register =
        ((val < 0) << 2) |
        ((val == 0) << 1) |
        (val > 0);
}

void tick()
{
	// fetch
	
	current_instruction = memory[pc];
	pc++; // update the pc after retriving the instruction (bob style)
	uint8_t opcode = (current_instruction >> 12) & 0xF;
	uint8_t dst = (current_instruction >> 9) & 0x7;
	
	switch (opcode)
	{
		/*
		NOP,
		ADD,
		AND,
		NOT,
		LD,
		LDI,
		LDR,
		ST,
		STI,
		STR,
		BR,
		JMP,
		JSR,
		LEA,
		RET,
		TRAP
		*/
		case NOP:
			break;

		case ADD:
			switch ((current_instruction >> 7) & 0x3) 
			{
				case 0:
					if (current_instruction & 0x1) 
					{
						printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
						exit(0xdeadbeef);
					}
					registers[dst] = registers[(current_instruction >> 4) & 0x7] + registers[(current_instruction >> 1) & 0x7];
					break;
					
				case 1: // i have no idea why this exist why would anyone use like 4 bit signed imm :sob:
					registers[dst] = registers[((current_instruction >> 4) & 0x7)] + sext_4((current_instruction & 0xF));
					break;

				case 2:
					if (current_instruction & 0xF) {
						printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
						exit(0xdeadbeef);
					}

					registers[dst] = registers[(current_instruction >> 9) & 0x7] + registers[(current_instruction >> 4) & 0x7];
					break;
				case 3:
					registers[dst] = registers[(current_instruction >> 9) & 0x7] + sext_7((current_instruction & 0x7F));
					break;
					
				
			}
			updateCC(registers[dst]);
			break;
			
		case AND:
			switch ((current_instruction >> 7) & 0x3) 
			{
				case 0:
					if (current_instruction & 0x1) 
					{
						printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
						exit(0xdeadbeef);
					}
					registers[dst] = registers[(current_instruction >> 4) & 0x7] & registers[(current_instruction >> 1) & 0x7];
					break;
					
				case 1: // i have no idea why this exist why would anyone use like 4 bit signed imm :sob:
					registers[dst] = registers[((current_instruction >> 4) & 0x7)] & sext_4((current_instruction & 0xF));
					break;

				case 2:
					if (current_instruction & 0xF) {
						printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
						exit(0xdeadbeef);
					}

					registers[dst] = registers[(current_instruction >> 9) & 0x7] & registers[(current_instruction >> 4) & 0x7];
					break;
					
				case 3:
					registers[dst] = registers[(current_instruction >> 9) & 0x7] & sext_7((current_instruction & 0x7F));
					break;
					
			}
			updateCC(registers[dst]);
			break;

		case NOT:
			if (current_instruction & 0x1F) {
				printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
				exit(0xdeadbeef);
			}

			if (current_instruction & 0x100) {
				registers[dst] = ~registers[(current_instruction >> 9) & 0x7];
			} else {
				registers[dst] = ~registers[(current_instruction >> 5) & 0x7];
			}
			updateCC(registers[dst]);
			break;

		case LD:
			// ram.mar = pc + sext(current_instruction & 0x1FF, 9);
			// registers[dest] = ram.mdr;
			// updateCC(registers[dest]);
			registers[dst] = memory[pc + sext_9(current_instruction & 0x1ff)];
			updateCC(registers[dst]);
			break;

		case LDI:
			/* ldi , sti
			ram.mar = pc + sext(current_instruction & 0x1FF, 9);
			ram.mdr = ram.memory[ram.mar];
			ram.mar = ram.mdr;
			*/
			/*
			registers[dest] = ram.mdr;
			updateCC(registers[dest]);
			*/
			registers[dst] = memory[memory[pc + sext_9(current_instruction & 0x1FF)]];
			updateCC(registers[dst]);
			break;

		case LDR:
			// ram.mar = registers[(current_instruction >> 6) & 0x7] + sext(current_instruction & 0x3F, 6);
			registers[dst] = memory[registers[(current_instruction >> 6) & 0x7] + sext_6(current_instruction & 0x3F)];
			updateCC(registers[dst]);
			break;

		case ST:
			memory[pc + sext_9(current_instruction & 0x1FF)] = registers[dst];
			break;

		case STI:
			memory[memory[pc + sext_9(current_instruction & 0x1FF)]] = registers[dst];
			break;
		
		case STR:
			memory[registers[(current_instruction >> 6) & 0x7] + sext_6(current_instruction & 0x3F)] = registers[dst];
			break;

		case BR:
			if ((current_instruction >> 9) & flag_register)
			{
				pc += sext_9(current_instruction & 0x1FF);
			}
			break;
			
		case JMP:
			if (current_instruction & 0x1FF) {
				printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
				exit(0xdeadbeef);
			}

			pc = registers[dst];
			break;

		case JSR:
			registers[7] = pc;
			if (!(current_instruction & 0x800)) 
				pc += sext_11(current_instruction & 0x7FF);
				
			else 
				pc = registers[(current_instruction >> 8) & 0x7];
				
			break;

		case LEA:
			registers[dst] = pc + sext_9(current_instruction & 0x1FF);
			updateCC(registers[dst]);
			break;

		case RET:
			pc = registers[7];
			break;
		case TRAP:
			if (current_instruction & 0xFF) {
				printf("bad instruction: %X\n at memory address %X\n", current_instruction, memory[pc - 1]);
				exit(-1);
			}

			switch ((current_instruction >> 8) & 0xF) {
				case HALT:
					exit(0);
					break;
					
				case PUTC:
					putchar(registers[0] & 0xFF);
					break;
					
				case PUTS:
					registers[7] = pc;
					pc = registers[0];
					while (current_instruction != 0) {
						current_instruction = memory[pc];
						registers[0] = current_instruction;
						putchar(registers[0] & 0xFF);
						pc++;					
					}

					putchar('\n');
					
					pc = registers[7];
					break;
					
				case GETS:
				{
					int n = registers[1];
					registers[7] = pc;
					char buf[n];
					fgets(buf, n, stdin);
					for (size_t i = 0; i < strlen(buf); i++) {
						memory[registers[0] + i] = buf[i] & 0xFF;
					}

					pc = registers[7];
					break;
				}
					
				case EXTEND:
					break;
			}

			break;
	}
		
}


int main(int argc, char **argv) // assembler havent rewriten yet
{
    
}
