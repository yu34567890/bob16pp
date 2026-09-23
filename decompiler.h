#ifndef DECOMPILER_H
#define DECOMPILER_H
#include <stdint.h>

char *decode(uint16_t ir);

#define sext_4(x)  (((x) >= 0x8)   ? (int16_t)((x) | 0xFFF0) : (x))
#define sext_6(x)  (((x) >= 0x20)  ? (int16_t)((x) | 0xFFC0) : (x))
#define sext_7(x)  (((x) >= 0x40)  ? (int16_t)((x) | 0xFF80) : (x))
#define sext_9(x)  (((x) >= 0x100) ? (int16_t)((x) | 0xFE00) : (x))
#define sext_11(x) (((x) >= 0x400) ? (int16_t)((x) | 0xF800) : (x))

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

#endif
