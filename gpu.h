#include <stdint.h>
#ifndef GPU_H
#define GPU_H
void gpu_init();
void gpu_render(uint16_t *vram);
void gpu_wait(int fps);
bool gpu_should_exit();

typedef enum {
	GPU_BASIC_TXT,
	GPU_COLOR_TXT,
	GPU_IDX_GRAPH,
	GPU_HCOLOR_GRAPH,
} GpuState;
#endif
