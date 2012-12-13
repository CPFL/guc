/*
 * main.c: Copyright (C) Shinpei Kato <shinpei@il.is.s.u-tokyo.ac.jp>
 */

#include "asminsn.h"
#include "mmio.h"
#include "regs.h"
#include "types.h"
#define FUC_NEWSCRATCH8 0x81c

unsigned int in_temp_cmd = 0x600dcafe;
unsigned int in_temp_data = 0x1111ffff;
unsigned int temp[5];

struct qe {
    uint32_t cmd;
    uint32_t data;
};

// 1dc
struct qe queue[8];
// 21c
int qget;
// 220
int qput;
// 224
struct qe q2[8];
// 264
int q2get=0;
// 268
int q2put=0;

void error(int ecode)
{
    // always inlined... and duplicated with 36e!
    mmio_write(FUC_NEWSCRATCH6, ecode);//add
    mmio_write(FUC_INTR_UP_SET, 1);
}

void enqueue(uint32_t cmd, uint32_t data)
{
    struct qe *ptr;

    mmio_write(FUC_NEWSCRATCH_SET2, 2);
    ptr = &queue[qput & 7];
    if ((qget & 7) == (qput & 7) && (qget & 8) != (qput & 8)) {
	error(0x12); // inline
    } else {
	ptr->data = data;
	ptr->cmd = cmd;
	if (++qput == 0x10)
	    qput = 0;
    }

    mmio_write(FUC_NEWSCRATCH_CLEAR2, 2);
}
void ihbody(void)
{
    int intr = mmio_read(FUC_INTR);
    if (intr & 4) { //fix this
	enqueue(mmio_read(0x068), mmio_read(0x064));
	mmio_write(0x074, 1);
    }
    if (intr & 0xff00) { //fix this
	if (intr & 0x100) { // auto ctx switch
	    enqueue(0x10000, 0);
	}
	if (intr & 0x200) {//fix this
	    enqueue(4, 0);
	}
	if (intr & 0x400) { // fwmthd fix this
	    enqueue(0x10002, 0);
	}
	if (intr & 0x8000) { // nop?!? fix this
	    enqueue(0x10003, 0);
	}
    }
    mmio_write(0x004, intr);
}

void mmwr(uint32_t addr, int val, uint32_t u1, uint32_t u2)
{
    extr(addr, addr, 2, 25);
    mmio_write(FUC_MMIO_WRVAL, val);
    mmio_write(FUC_MMIO_CTRL, (addr << 2) |
	    (u1 & 1) | 0xc0000000 | (!!u2) << 29);
    while (mmio_read(FUC_MMIO_CTRL) & 0x80000000);
    if (u2)
	while (!(mmio_read(FUC_DONE)&0x80));
}

int mmrd (uint32_t addr, uint32_t u1)
{
    extr(addr, addr, 2, 25);
    addr = addr << 2;
    mmio_write(FUC_MMIO_CTRL, addr  | (u1 & 1) | 0x80000000);
    while (mmio_read(FUC_MMIO_CTRL) & 0x80000000);
    while (!(mmio_read(FUC_DONE) & 0x40));
    return mmio_read(FUC_MMIO_RDVAL);
}

void set4160(void)
{
    mmio_write(FUC_NEWSCRATCH_SET3, 0x400);
    mmwr(0x404160, 1, 0, 0);
    while (!(mmrd(0x404160, 0) & 0x10));
    mmio_write(FUC_NEWSCRATCH_CLEAR3, 0x400);
}

void xfer(int base, int offset, int sizel2, int local_addr,
	int subtrg, int trg, int dir, int wait)
{
    int tspec;

    mmio_write(FUC_NEWSCRATCH_SET3, 0x200);
    switch (trg) {
	case 2:
	    tspec = 1;
	    break;
	case 1:
	    switch (subtrg) {
		case 0:
		    tspec = 0x80000002;
		    break;
		case 2:
		    tspec = 0x80000004;
		    break;
		case 3:
		    tspec = 0x80000003;
		    break;
		default:
		    tspec = 0x80000000;
		    break;
	    }
	    break;
	default:
	    tspec = 0;
	    break;
    }

    mmio_write(0xa20, tspec);
    set_xdbase(base);
    set_xtargets(0);
    mmio_write(0x10c, 0);

    if (dir == 1) {
	xdld(offset, (sizel2 << 0x10) | local_addr);
    } else {
	xdst(offset, (sizel2 << 0x10) | local_addr);
    }

    if (wait == 2)
	xdwait();
    mmio_write(0xa20, 0);
    mmio_write(FUC_NEWSCRATCH_CLEAR3, 0x200);	
}
void waitdone_12(void)
{
    mmio_write(FUC_NEWSCRATCH_SET3, 0x100);
    while (mmio_read(FUC_DONE) & 0x1000);
    mmio_write(FUC_NEWSCRATCH_CLEAR3, 0x100);
}
void setxferbase(int x)
{
    waitdone_12();
    mmio_write(0xa04, x);
}
void clear4160(void)
{
    mmio_write(FUC_NEWSCRATCH_SET3, 0x800);
    // waitdone_12();
    mmwr(0x404160, 0, 0, 0);
    mmio_write(FUC_NEWSCRATCH_CLEAR3, 0x800);
}

void work(void){

    uint32_t xdata;
    uint32_t cmd,data;
    mmio_write(FUC_NEWSCRATCH_SET2, 8);
    if (q2get != q2put) {
	struct qe *ptr = &q2[q2get & 7];
	cmd = ptr->cmd & ~0x7800;
	data = ptr->data;
	if (++q2get == 0x10)
	    q2get = 0;
    } else if (qget != qput) {
	struct qe *ptr = &queue[qget & 7];
	cmd = ptr->cmd & ~0x7800;
	data = ptr->data;
	if (++qget == 0x10)
	    qget = 0;
    } else {
	mmio_write(FUC_NEWSCRATCH_CLEAR2, 8);
	return;
    }

    in_temp_cmd = cmd;
    in_temp_data = data;


    set4160();
    waitdone_12();

   
	mmio_write(0xa24, 0);
	mmio_write(FUC_MEM_CHAN, 0); /* type=nv50_channnel????? */
	mmio_write(FUC_MEM_CMD, 7);
	while (mmio_read(FUC_MEM_CMD)&0x1f);

    setxferbase(10);
    extr(xdata,0x08020495,28,29);
    // base, 
    //xfer(0x80020494<<4,0,1,0,xdata,1,1,2);
    xfer(0x20495<<4,0,5,&temp[0],0,2,1,2);

    clear4160();
    mmio_write(0x808, 0xb0000000 | temp[0]);
    mmio_write(0x804, 0xb0000000 | temp[1]);
    mmio_write(0x80c, 0x12345678);


}
/*
 * entry point into the micrcontroller code.
 */
int main(void)
{
    int offset = 0;
    int sizel2 = 4;
    int local_addr = 0x400;
    int xdata;

    mmio_write(FUC_NEWSCRATCH_SET2,1);
    mmio_write(FUC_ACCESS_EN, FUC_AE_BIT_FIFO);
    mmio_write(FUC_INTR_DISPATCH, 0); // fuc interrupt 0
    mmio_write(FUC_INTR_EN_SET, 0x8704);
    mmio_write(0x00c, 4);

    set_flags(ie0);
    set_flags($p2);
    int i=0;

    mmio_write(0xc24, 0xffffffff);//INTR_UP_ENABLE??
    while(1){
	mmio_write(FUC_NEWSCRATCH4,0x87654321);
	sleep($p2);
	mmio_write(FUC_NEWSCRATCH5,++i);
	work();
	set_flags($p2);
    }
    return 0;
}
