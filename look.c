#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gpu.h"

// CURRENT GPU STATUS:
// 4kw of memory
// vram[0] is the mode of the gpu
// rest is up to the mode
// Modes:
// 0: Basic txt,
//  145x55 character display,
//  8 bit per char, packed
// 1: Color txt,
//  100x40 character display,
//  16 bit per char
//  upper 8 bits in bbbgggrr format for color
//  low 8 bits character represenation
//  bg assumed pure black

void mode0_test() {
	uint16_t vram[4096];
	vram[0] = 0;

	char *str = "\xE0\xE1\n\xF0\xF1";
	memcpy(&vram[1], str, strlen(str));

	while (1) {
		gpu_render(vram);
		gpu_wait(60);
		if (gpu_should_exit()) break;
	}
}

void mode1_test() {
	uint16_t vram[4096];
	vram[0] = 1;

	uint8_t *str = "\x03""R""\x1C""G""\xE0""B";
	memcpy(&vram[1], str, strlen(str));

	while (1) {
		gpu_render(vram);
		gpu_wait(60);
		if (gpu_should_exit()) break;
	}
}

void mode2_test() {
	uint16_t vram[4096];
	vram[0] = 2;

	for (int i = 0; i < 160; ++i) {
		for (int j = 0; j < 100; ++j) {
			int idx = 160 * j + i + 1;
			if ((i + j) & 1) {
				vram[idx] = 16;
			} else {
				vram[idx] = 0;
			}
		}
	}

	while (1) {
		gpu_render(vram);
		gpu_wait(60);
		if (gpu_should_exit()) break;
	}
}

int main() {
	gpu_init();
	mode1_test();
}
