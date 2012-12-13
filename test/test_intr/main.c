/*
 * main.c: Copyright (C) Yusuke Fujii <yukke@ubi.cs.ritsumei.ac.jp>
 */

#include "asminsn.h"
#include "mmio.h"
#include "regs.h"
#include "types.h"

int i;

void ihbody(void)
{
    u32 intr = mmio_read(0x008);
    if ( intr & 4){
   	i+=1;
    }
	mmio_write(0x74,1);
    mmio_write(0x004,0);
    
    return;
}

/*
 * entry point into the microcontroller code.
 */
int main(void)
{
    int j;
    i=0;
    j=0;
    volatile int v;
    mmio_write(FUC_ACCESS_EN, FUC_AE_BIT_FIFO);
    mmio_write(FUC_NEWSCRATCH_SET2,1);
    mmio_write(FUC_NEWSCRATCH_CLEAR3,0xffffffff);  
    mmio_write(FUC_NEWSCRATCH_CLEAR4,0xffffffff);  
    //use iv0
    mmio_write(FUC_INTR_DISPATCH, 0); // fuc interrupt 0
    mmio_write(FUC_INTR_EN_SET, 0x8704);
    mmio_write(0x00c, 4);

#if 0
    mmio_write_i(FUC_MMCTX_INTR_ROUTE, 0, 0x2003);
    mmio_write_i(FUC_MMCTX_INTR_ROUTE, 1, 0x2004);
    mmio_write_i(FUC_MMCTX_INTR_ROUTE, 2, 0x200b);
    mmio_write_i(FUC_MMCTX_INTR_ROUTE, 7, 0x200c);
    mmio_write_i(0xc14, 0x10, 0x202d);
#endif
    set_flags($p2);
    set_flags(ie0);

    
    //mmio_write(FUC_NEWSCRATCH1,0x600dc0de);

     mmio_write(0xc24, 0xffffffff);//INTR_UP_ENABLE??
    
     volatile int z;

    while(1) {
	mmio_write(FUC_NEWSCRATCH4,i);	
	mmio_write(FUC_NEWSCRATCH5,j++);
	sleep($p2);	
    	set_flags($p2);
    }
    
    return 0;
}

