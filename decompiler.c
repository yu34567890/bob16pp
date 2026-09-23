#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "decompiler.h"
// no strlen used in this project
// because why not 



char *instruction_string(uint16_t ir)
{
	uint16_t instruction_opcode = ir >> 12;
	switch (instruction_opcode)
	{
		case NOP: return "nop" ;
		case ADD: return "add" ;
		case AND: return "and" ;
		case NOT: return "not" ;
		case LD:  return "ld " ;
		case LDI: return "ldi" ;
		case LDR: return "ldr" ;
		case ST:  return "st " ;
		case STI: return "sti" ; 
		case STR: return "str" ;
		case BR:  return "br " ;
		case JMP: return "jmp" ;
		case JSR: return "jsr" ;
		case LEA: return "lea" ;
		case RET: return "ret" ; 
		case TRAP:return "trp" ;
		default: return "unknow";
	}
}

char *decode(uint16_t ir)
{
	char *instruction = instruction_string(ir);
	INSTRUCTION opcode = ir>>12;
	char *result = calloc(32, sizeof(char)); // largest possible instruction in string

	
	char input_registers[9] = "R  R  R ";

	memcpy(result, instruction, 3);
	result[3] = ' ';

	switch (opcode)
	{
		case NOP:
			return instruction;

		case ADD:

			switch ((ir>>7) & 0x3)
			{
				case 0: // add dst src1 src2
					input_registers[1] = ((ir>>9) & 0x7) + '0';
					input_registers[4] = ((ir>>4) & 0x7) + '0';
					input_registers[7]  = ((ir>>1) & 0x7) + '0';
					memcpy(result+4, input_registers, 9);
					break;
				
				case 1: // add dst src1 4bit imm
					input_registers[1] = ((ir>>9) & 0x7) + '0';
					input_registers[4] = ((ir>>4) & 0x7) + '0';
					memcpy(result+4, input_registers, 5);
					sprintf(result+10, "%d", sext_4(ir & 0xf));
					break;

				case 2:// add dst src1
					input_registers[1] = ((ir>>9) & 0x7) + '0';
					input_registers[4] = ((ir>>4) & 0x7) + '0';
					memcpy(result+4, input_registers, 5);
					break;

				case 3:// add dst, 7 bit imm
				       input_registers[1] = ((ir>>9) & 0x7) + '0';
				       memcpy(result+4, input_registers, 3);
				       sprintf(result+7, "%d", sext_7(ir & 0x7f));
				       break;
			}
			break;

		case AND:

			switch ((ir>>7) & 0x3)
			{
				case 0: // add dst src1 src2
					input_registers[1] = ((ir>>9) & 0x7) + '0';
					input_registers[4] = ((ir>>4) & 0x7) + '0';
					input_registers[7]  = ((ir>>1) & 0x7) + '0';
					memcpy(result+4, input_registers, 9);
					break;
				
				case 1: // add dst src1 4bit imm
					input_registers[1] = ((ir>>9) & 0x7) + '0';
					input_registers[4] = ((ir>>4) & 0x7) + '0';
					memcpy(result+4, input_registers, 5);
					sprintf(result+10, "%d", sext_4(ir & 0xf));
					break;

				case 2:// add dst src1
					input_registers[1] = ((ir>>9) & 0x7) + '0';
					input_registers[4] = ((ir>>4) & 0x7) + '0';
					memcpy(result+4, input_registers, 5);
					break;

				case 3:// add dst, 7 bit imm
				       input_registers[1] = ((ir>>9) & 0x7) + '0';
				       memcpy(result+4, input_registers, 3);
				       sprintf(result+7, "%d", sext_7(ir & 0x7f));
				       break;
			}
			break;

		case NOT:	
			if (ir & 0x100) // not dst
			{
				input_registers[1] = ((ir>>9) & 0x7) + '0';
				memcpy(result+4, input_registers, 3);	
			} 
			else // not dst src
			{
				input_registers[1] = ((ir>>9) & 0x7) + '0';
				input_registers[4] = ((ir>>5) & 0x7) + '0';
				memcpy(result+4, input_registers, 5);
				
			}
			break;

		case LD:  
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			memcpy(result+4, input_registers, 3);
			sprintf(result+7, "%d", sext_9(ir&0x1ff));
			break;

		case LDI:
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			memcpy(result+4, input_registers, 3);
			sprintf(result+7, "%d", sext_9(ir&0x1ff));
			break;		

		case LDR: 
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			input_registers[4] = ((ir>>4) & 0x7) + '0';
			memcpy(result+4, input_registers, 5);
			break;
			
		case ST:  
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			memcpy(result+4, input_registers, 3);
			sprintf(result+7, "%d", sext_9(ir&0x1ff));
			break;
		
		case STI: 
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			memcpy(result+4, input_registers, 3);
			sprintf(result+7, "%d", sext_9(ir&0x1ff));
			break;

		case STR:
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			input_registers[4] = ((ir>>4) & 0x7) + '0';
			memcpy(result+4, input_registers, 5);
			break;

		case BR: 
			result[4] = ((ir>>9) & 0x7) + '0';
			result[5] = ' '; 
			sprintf(result+6, "%d", sext_9(ir&0x1ff));
			break;

		case JMP: 
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			memcpy(result+4, input_registers, 3);
			break;

		case JSR:
			if (!(ir & 0x800))
			{
				sprintf(result+4, "%d", sext_11(ir&0x7ff));
			}
			else 
			{
				input_registers[1] = ((ir>>9) & 0x7) + '0';
				memcpy(result+4, input_registers, 3);
				break;
			}


			break;

		case LEA:
			input_registers[1] = ((ir>>9) & 0x7) + '0';
			memcpy(result+4, input_registers, 3);
			sprintf(result+8, "%d", sext_9(ir&0x1ff));
			break;

		case RET:  
			break;

		case TRAP:
			break;
		default: 
			return instruction;
	}
	return result; 
}


