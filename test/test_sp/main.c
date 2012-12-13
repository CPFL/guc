/*
 * main.c: Copyright (C) Shinpei Kato <shinpei@il.is.s.u-tokyo.ac.jp>
 */

#include "asminsn.h"
#include "mmio.h"
#include "regs.h"
#include "types.h"

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

    ihbody();
    
    for (i = 0; i < 10; i++) {
	mmio_write(0x40, array[i]);
	mmio_write(0x44, array[i]);
    }

    test(array[0], array[1], array[2], array[3]);

    return 0;
}
