/*
 * main.c: Copyright (C) Shinpei Kato <shinpei@il.is.s.u-tokyo.ac.jp>
 * 			 Yusuke Fuji <yukke@ubi.cs.ritsumei.ac.jp>
 */

#include "asminsn.h"
#include "mmio.h"
#include "regs.h"
#include "types.h"

int array[10];
int j=0,k=0;
void ihbody(void)
{
    unsigned int intr = mmio_read(0x08);
    if(intr & 0x8){
	mmio_write(0x804,mmio_read(0x0804)+1);
    }
    j++;
    mmio_write(0x74,1);
    mmio_write(0x04,intr);
}
void ihbody1(void)
{
    unsigned int intr = mmio_read(0x08);
 
    mmio_write(0x04,0x1);
    //if(intr == 0x1){
	mmio_write(0x800,mmio_read(0x0800)+1);
    //}
    k++;
    //mmio_write(0x28,1);
}


/*
 * entry point into the microcontroller code.
 */
int main(void)
{
    int i=0;
    mmio_write(FUC_ACCESS_EN,FUC_AE_BIT_FIFO);
    mmio_write(0x04,0xffffffff);//intr_line all reset
   mmio_write(0x0c,0xfc04);
    mmio_write(0x20,0xfffffff);// timer period
    mmio_write(0x24,0xfffffff);// current timer
    mmio_write(0x28,0x1); //timer enabled

    //use iv0 & 1, intr_line[0]=iv0,intr_line[1]=iv1 
    mmio_write(FUC_INTR_DISPATCH,0x00010000); //0-15bit => 0bit 16-31bit => 1bit 
    mmio_write(FUC_INTR_EN_SET, 0x8705); // intrrupt_line mask set

    set_flags(ie1);//ihbody1
    set_flags(ie0);//ihbody
    //mmio_write(0xc24, 0xffffffff);//INTR_UP_ENABLE??
    set_flags($p2);

    while(1){
	sleep($p2);
	mmio_write(0x814,j);
	mmio_write(0x810,k);
   }

    return 0;
}
