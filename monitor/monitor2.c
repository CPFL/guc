#include "libio.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NVC0_CTXCTL_FUC_BASE09         0x409000
#define NVC0_CTXCTL_FUC_BASE1A         0x41a000
#define NVC0_DAMEON_FUC_BASE         0x10a000

#define FUC_REG_SCRATCH1 0x040
#define FUC_REG_SCRATCH2 0x044
#define FUC_REG_SCRATCH3 0x080
#define FUC_REG_RETURN 0x800

#define FUC_REG_SCRATCH4	 0x084
#define WRCMD 0x504
#define WRDATA 0x500
#define COM_SIZE 0x256
#define CC_SCRATCH_CLEAR0 0x840

#define MAX_CODE_SIZE     0x110000
#define MAX_DATA_SIZE     0x110000

#define FALSE 0
#define TRUE 1

#define mmio_read09(addr) \
    NV_RD32(NVC0_CTXCTL_FUC_BASE09 + (addr))
#define mmio_write09(addr, val)				\
    NV_WR32(NVC0_CTXCTL_FUC_BASE09 + (addr), (val))
#define mmio_wait09(addr, val)				\
    NV_WAIT(NVC0_CTXCTL_FUC_BASE09 + (addr), 0x200000ULL, (val))

#define mmio_wait09_neq(addr,mask,val) \
    NV_WAIT_NEQ(NVC0_CTXCTL_FUC_BASE09 + (addr), 0x200000ULL,(mask), (val))

#define mmio_read1a(addr) \
    NV_RD32(NVC0_CTXCTL_FUC_BASE1A + (addr))
#define mmio_write1a(addr, val)				\
    NV_WR32(NVC0_CTXCTL_FUC_BASE1A + (addr), (val))
#define mmio_wait1a(addr, val)				\
    NV_WAIT(NVC0_CTXCTL_FUC_BASE1A + (addr), ~0, (val))


/* define area  */
//#define PRINT_PCOUNTER
//#define PRINT_PDAEMON
#define PRINT_PGRAPH_GPC



int global_time=0;
int finish_flg=0;
#define PRINT_SIZE 0x400
#define PRINT_LINE_VALUE 9
#define PRINT_PERIODIC 5000//1000000

int reg_hub[0x1000];
int reg_gpc[4][0x1000];

void nvc0_debugunit(int base,int *reg){
    int i=0,j=0;
    int off=0;
    int temp;
    int k=0;

#if 0
    printf("PGRAPH: %06x - scratch 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x40), NV_RD32(base + 0x44),NV_RD32(base + 0x80),NV_RD32(base + 0x84));
    printf("PGRAPH: %06x - done 0x%08x wrval 0x%08x ctrl 0x%08x\n",base,NV_RD32(base + 0x400),NV_RD32(base + 0x730),NV_RD32(base + 0x728));;
    printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x800), NV_RD32(base + 0x804),NV_RD32(base + 0x808),NV_RD32(base + 0x80c));
    printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x810), NV_RD32(base + 0x814),NV_RD32(base + 0x818),NV_RD32(base + 0x81c));
    printf("PGRAPH: %06x - xfer 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x110), NV_RD32(base + 0x114),NV_RD32(base + 0x118),NV_RD32(base + 0x11c));
    printf("PGRAPH: %06x - MEM BASE:0x%08x CHAN: 0x%08x CMD:0x%08x TG: 0x%08x\n",base,NV_RD32(base + 0xa04), NV_RD32(base + 0xa0c),NV_RD32(base + 0xa10),NV_RD32(base + 0xa20));
#else 
    for(i=0,off=0;i< PRINT_SIZE+off && i<0xc94;){
	for(k=0;k<PRINT_LINE_VALUE*4 && i < 0xc94;i+=4){
	    /* mmio read fault check*/
	    if((i >= 0x508 && i < 0x600) || i== 0x128){
	    }else{
		/**/
		if((temp=NV_RD32(base+i)) == reg[i/4]){
		    off+=4;
		}else{
		    if(base<0x41a000){
			printf("[%04x]%08x, ",i,temp);
		    }else{
			printf("[g%04x]%08x, ",i,temp);
		    }
		    reg[i/4]=temp;
		    k+=4;
		    j++;
		}
	    }
	}
	//i+=j;
	if(j!=0)puts("");
    }

    if(j>0)printf("PRINT_VALUE [%08x]\n",global_time);
#endif
}


void nvc0_debugunit_gpc(int base,int *reg){
    int i=0,j=0;
    int off=0;
    int temp;
    int k=0;
    for(k=0x800,i=0;k<0x820;k+=4,i++){
	/* mmio read fault check*/

	if((temp=NV_RD32(base + k)) != reg[i] ){//base+k)) != reg[(k-0x800) /4]){
	    printf("[g%04x]%08x, ",k,temp);
	    reg[i]=temp;
	    j++;

	}

	}
	/* mmio read fault check*/
	k=0x400;
	if((temp=NV_RD32(base + k)) != reg[i] ){//base+k)) != reg[(k-0x800) /4]){
	    printf("[g%04x]%08x, ",k,temp);
	    reg[i]=temp;
	    j++;

	}
	i++;
	k=0x430;
	if((temp=NV_RD32(base + k)) != reg[i] ){//base+k)) != reg[(k-0x800) /4]){
	    printf("[g%04x]%08x, ",k,temp);
	    reg[i]=temp;
	    j++;

	}
	i++;
	k=0x120;
	if((temp=NV_RD32(base + k)) != reg[i] ){//base+k)) != reg[(k-0x800) /4]){
	    printf("[g%04x]%08x, ",k,temp);
	    reg[i]=temp;
	    j++;

	}
	if(j!=0)puts("");
	}
	/*
	   void nvc0_debugunit_gpc(int base){
	   int i=0,j=0;
	   int off=0;
	   int temp;
	   int k=0;
	   printf("PGRAPH: %06x - scratch 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x40), NV_RD32(base + 0x44),NV_RD32(base + 0x80),NV_RD32(base + 0x84));
	   printf("PGRAPH: %06x - done 0x%08x wrval 0x%08x ctrl 0x%08x\n",base,NV_RD32(base + 0x400),NV_RD32(base + 0x730),NV_RD32(base + 0x728));;
	   printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x800), NV_RD32(base + 0x804),NV_RD32(base + 0x808),NV_RD32(base + 0x80c));
	   printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x810), NV_RD32(base + 0x814),NV_RD32(base + 0x818),NV_RD32(base + 0x81c));
	   printf("PGRAPH: %06x - xfer 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x110), NV_RD32(base + 0x114),NV_RD32(base + 0x118),NV_RD32(base + 0x11c));
	   printf("PGRAPH: %06x - MEM BASE:0x%08x CHAN: 0x%08x CMD:0x%08x TG: 0x%08x\n",base,NV_RD32(base + 0xa04), NV_RD32(base + 0xa0c),NV_RD32(base + 0xa10),NV_RD32(base + 0xa20));
	   }
	   */
	void nvc0_debug_pcounter(){
	    int base =0x180000;
	    base += 0x30000;
	    printf("PCOUNTER: %06x - status 0x%08x:0x%08x:0x%08x:0x%08x\n"
		    ,base,NV_RD32(base + 0xe4),NV_RD32(base + 0x040),NV_RD32(base + 0x4c),NV_RD32(base+0x50) );;
	    printf("PCOUNTER: %06x - status 0x%08x:0x%08x:0x%08x:0x%08x\n"
		    ,base,NV_RD32(base + 0x54),NV_RD32(base + 0x058),NV_RD32(base + 0x5c),NV_RD32(base+0x60) );;
	    printf("PCOUNTER: %06x - status 0x%08x:0x%08x:0x%08x:0x%08x\n"
		    ,base,NV_RD32(base + 0x64),NV_RD32(base + 0x68),NV_RD32(base + 0x6c),NV_RD32(base+0xec) );;
	}

	void nvc0_debug(){
	    int i = NV_RD32(0x40904c);
	    if( NV_RD32(0x409500) == 0xbad && finish_flg )exit(1);    
	    else if( NV_RD32(0x409500) != 0xbad)finish_flg = 1;
	    /*
	       printf("MicroController shows register: %08d \n",global_time++);
	       printf("CMD(504) = %08x, DATA(500) = %08x\n",NV_RD32(0x409504),NV_RD32(0x409500));
	       printf(" CMD(68) = %08x, DATA(64)  = %08x\n",NV_RD32(0x409068),NV_RD32(0x409064));
	       printf("INTR EN_ST(18) = %08x, INTR_MASK(1c)  = %08x\n",NV_RD32(0x409018),NV_RD32(0x40901c));
	       printf("INTR TIMER(24) = %08x, TI_ENABLE(28)  = %08x\n",NV_RD32(0x409024),NV_RD32(0x409028));
	       printf("INTR    ST(08) = %08x, TI_MODE  (0c)  = %08x\n",NV_RD32(0x409008),NV_RD32(0x40900c));
	       printf("-HUB- busy? %d\n",i&0x1);

*/
	    global_time++;
	    nvc0_debugunit(0x409000,reg_hub);
#ifdef PRINT_PCOUNTER
	    nvc0_debug_pcounter();
#endif
#ifdef PRINT_PDAEMON
	    // printf("\n-PDAEMON-\n");
	    // nvc0_debugunit(0x10a000);
#endif
#ifdef PRINT_PGRAPH_GPC
	    nvc0_debugunit_gpc(0x502000,reg_gpc[0]);
	    nvc0_debugunit_gpc(0x50a000,reg_gpc[1]);
	    nvc0_debugunit_gpc(0x512000,reg_gpc[2]);
	    nvc0_debugunit_gpc(0x51a000,reg_gpc[3]);
	    //nvc0_debugunit(0x50a000);
	    //nvc0_debugunit(0x512000);
	    //nvc0_debugunit(0x51a000);
#endif
	}

	int init_device(void)
	{
	    /* init PCI. */
	    init();

	    //  /* reset engine. */
	    //  NV_WR32(0x200, 0xfffd3ffd);
	    //  NV_WR32(0x200, 0xffffffff);
	    //  NV_WR32(0x200, NV_RD32(0x200) & 0xffffefff);
	    //  NV_WR32(0x200, NV_RD32(0x200) | 0x00001000);
	    // sleep(1);

	    return TRUE;
	}

	void fini_device(void)
	{
	    fini();
	}

	int main(int argc, char **argv)
	{
	    int print_flg=0;
	    int i;
	    char def[1]="a";
	    if(argv[1] == NULL){
		printf("usage: monitor <time> [-a]\nDefault time is 500msec\n");
	    }else if(strcmp(argv[1], "-a") == 0 ){
		print_flg=1;
		printf("aiueo");
	    }

	    // fini_device();

	    /* init the device. you can't access the fuc until you do this. */
	    init_device();

	    printf("Start monitored Microcontroller\n");
	    for(i=0;i<0x1000;i++){
		reg_hub[i]=0;
		reg_gpc[0][i]=0;
		reg_gpc[1][i]=0;
		reg_gpc[2][i]=0;
		reg_gpc[3][i]=0;
	    } 


	    while(1){
		/*
		   if(!print_flg){
		   printf("\x1B[2J");
		   }else{
		   printf("\n");
		   }
		   */
		usleep(PRINT_PERIODIC);
		nvc0_debug();
	    }



	    /* finish the device. */
	    fini_device();

	    return 0;
	}

