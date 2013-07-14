/*
 * main.c: Copyright (C) Shinpei Kato <shinpei@il.is.s.u-tokyo.ac.jp>
 */

#include "asminsn.h"
#include "mmio.h"
#include "regs.h"
#include "types.h"


#define FUC_REG_SCRACH1 0x40
#define FUC_REG_SCRACH2 0x44
#define FUC_REG_SCRACH3 0x80
#define FUC_REG_SCRACH4 0x88



int array[10];

void ihbody(void)
{
    int i;
    for (i = 0; i < 10; i++)
	array[i] = i;
}

void test(int a, int b, int c, int d)
{
    int i;
    for (i = a; i < b; i++)
	array[i] = c + d;
}

/*
 * entry point into the microcontroller code.
 */
int main(void)
{
    int i;
    int reg40, reg44;

    mmio_write(0x40, 1234);
    mmio_write(0x44, 0x1234);
    reg40 = mmio_read(0x40);
    reg44 = mmio_read(0x44);
    test(array[0], array[1], array[2], array[3]);

    mmio_write(0x40, reg40);
    mmio_write(0x44, reg44);

    return 0;
}
