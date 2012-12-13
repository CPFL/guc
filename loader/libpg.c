#include "libio.h"
#include "libpg.h"
#include <stdint.h>

void nv50_graph_reset() {
	uint32_t enable = NV_RD32(0x200);
	NV_WR32(0x200, enable & ~0x1000);
	NV_WR32(0x200, enable | 0x1000);
	NV_WR32(0x400040, -1);
	NV_WR32(0x400040, 0);
}
