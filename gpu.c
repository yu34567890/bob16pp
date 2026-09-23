#include <stdint.h>
#include <stdlib.h>
#include <endian.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "gpu.h"

#include <SDL3/SDL.h>
#define SCL 4
#define CHRW 4
#define CHRH 6

#include "gpudat.h"

SDL_Window *gpu_window;
SDL_Renderer *gpu_renderer;

#define BTXT_WIDTH 80
#define BTXT_HEIGHT 30

#define CTXT_WIDTH 80
#define CTXT_HEIGHT 30

void plt_pix(int x, int y, int r, int g, int b) {
	SDL_SetRenderDrawColor(gpu_renderer, r, g, b, 255);

	SDL_FRect rectangle = {
		.x = SCL*x,
		.y = SCL*y,
		.w = SCL,
		.h = SCL,
	};
	SDL_RenderFillRect(gpu_renderer, &rectangle);
}

void write_bchr(uint8_t chr, int x, int y) {
	int tx = x * CHRW;
	int ty = y * CHRH;

	uint32_t chrtex = charfont[chr];

	for (int i = 0; i < CHRH; ++i) {
		for (int j = 0; j < CHRW; ++j) {
			int idx = j + i * CHRW;
			uint32_t bitm = (1 << (23-idx));
			if (!(chrtex & bitm)) continue;
			plt_pix(tx+j, ty+i, 20, 200, 20);
		}
	}
}

void write_btxt(uint8_t *txt, size_t len) {
	int x = 0;
	int y = 0;
	for (int i = 0; i < len; ++i) {
		if(txt[i] == '\n' || x >= BTXT_WIDTH) {
			++y;
			x = 0;
			if (txt[i] == '\n') continue;
		}
		write_bchr(txt[i], x, y);
		++x;
	}
}

void write_cchr(uint16_t chr, int x, int y) {
	int tx = x * CHRW;
	int ty = y * CHRH;

	chr = htobe16(chr);
	//printf("0x%04X\n", chr);
	uint8_t col = chr >> 8;
	uint8_t chrcode = chr & 0xFF;
	uint8_t r = col & 0x3;
	r <<= 6;
	uint8_t g = (col>>2) & 0x7;
	g <<= 5;
	uint8_t b = (col>>5) & 0x7;
	b <<= 5;
	uint32_t chrtex = charfont[chrcode];

	printf("%d %d %d\n", r, g, b);

	for (int i = 0; i < CHRH; ++i) {
		for (int j = 0; j < CHRW; ++j) {
			int idx = j + i * CHRW;
			uint32_t bitm = (1 << (23-idx));
			if (!(chrtex & bitm)) continue;
			plt_pix(tx+j, ty+i, r, g, b);
		}
	}
}

void write_ctxt(uint16_t *txt, size_t len) {
	int x = 0;
	int y = 0;
	for (int i = 0; i < len; ++i) {
		if(txt[i] == '\n' ||x  >= CTXT_WIDTH) {
			++y;
			x = 0;
			if (txt[i]&0xFF == 0x0A) continue;
		}
		write_cchr(txt[i], x, y);
		++x;
	}
}

void gpu_init() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer(
		"BOB16pp Graphics",
		SCL*80*4,
		SCL*30*6,
		0,
		&gpu_window,
		&gpu_renderer
	);
}

void gpu_destroy() {
	SDL_DestroyWindow(gpu_window);
	SDL_Quit();
}

bool gpu_should_exit() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT: {
				return true;
				break;
			}
			default: { break; }
		}
	}
	return false;
}

void gpu_render(uint16_t *vram) {
	SDL_SetRenderDrawColor(gpu_renderer, 0, 0, 0, 255);
	SDL_RenderClear(gpu_renderer);
	
	uint16_t mode = vram[0];
	switch (mode) {
		case GPU_BASIC_TXT: {
			SDL_SetWindowSize(gpu_window, SCL*BTXT_WIDTH*4, SCL*BTXT_HEIGHT*6);
			void *string = &vram[1];
			int stringlen = BTXT_WIDTH * BTXT_HEIGHT;
			write_btxt(string, stringlen);
			SDL_RenderPresent(gpu_renderer);
			break;
		}
		case GPU_COLOR_TXT: {
			SDL_SetWindowSize(gpu_window, SCL*CTXT_WIDTH*4, SCL*CTXT_HEIGHT*6);
			void *string = &vram[1];
			int stringlen = CTXT_WIDTH * CTXT_HEIGHT;
			write_ctxt(string, stringlen);
			SDL_RenderPresent(gpu_renderer);
			break;
		}
		default: {
			fprintf(stderr, "Fuck you %d\n", __LINE__);
			exit(1);
		}
	}
}

void gpu_wait(int fps) {
	SDL_Delay(1000/fps);
}
