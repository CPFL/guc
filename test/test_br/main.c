/*
 * main.c: Copyright (C) Shinpei Kato <shinpei@il.is.s.u-tokyo.ac.jp>
 */

#include "asminsn.h"
#include "mmio.h"
#include "regs.h"
#include "types.h"

void ihbody(void)
{
}

/*
 * entry point into the microcontroller code.
 */
int main(void)
{
    unsigned int i, j;
    
    mmio_write(0x40, 100);
    mmio_write(0x44, 200);    

    i = mmio_read(0x40);
    j = mmio_read(0x44);

    if (i != j) {
	mmio_write(0x44, 0xcafebabe);
    }
    
    return 0;
}
