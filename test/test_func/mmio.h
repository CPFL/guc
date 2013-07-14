#ifndef __FUC_IO_H__
#define __FUC_IO_H__

#include "types.h"

static inline void mmio_write(u32 addr, u32 val)
{
    __asm__ volatile ("iowr\tI[%0] %1" :: "r"(addr << 6), "r"(val));
}

static inline u32 mmio_read(u32 addr)
{
    int x;
    __asm__ volatile ("iord\t%0 I[%1]" :"=r"(x) : "r"(addr << 6));
    return x;
}

static inline void mmio_write_i(u32 addr, u32 i, u32 val)
{
    __asm__ volatile ("iowr\tI[%0] %1" :: "r"(addr << 6 + i * 4), "r"(val));
}

static inline u32 mmio_read_i(u32 addr, u32 i)
{
    int x;
    __asm__ volatile ("iord\t%0 I[%1]" :"=r"(x) : "r"(addr << 6 + i * 4));
    return x;
}

#endif
