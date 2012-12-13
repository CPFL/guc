#include "libio.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NVC0_CTXCTL_FUC_BASE09         0x409000
#define NVC0_CTXCTL_FUC_BASE1A         0x41a000

#define FUC_REG_SCRATCH1 0x040
#define FUC_REG_SCRATCH2 0x044
#define FUC_REG_SCRATCH3 0x080
#define FUC_REG_RETURN 0x800

/* yukke added :120507  */
#define FUC_REG_SCRATCH4	 0x084
#define WRCMD 0x504
#define WRDATA 0x500
#define COM_SIZE 0x256
#define CC_SCRATCH_CLEAR0 0x840
/**/

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

/*
 * if you want use ctxctl.h firmware microcode, activate the macro
 * #define USE_BLOB_UCODE
 */

//#define USE_BLOB_UCODE

#include<nvc0_ctxctl.gpc.h>

static void
nvc0_graph_load_microcode()
{
	int i;
	const uint32_t val260 = NV_RD32(0x260);

        printf("micro code loading...\n");
        NV_WR32(0x260, val260 & ~1);

#ifdef USE_BLOB_UCODE
       printf("use blob_ucode^\n");
        /* load HUB microcode */
       NV_WR32(0x4091c0, 0x01000000);
        for (i = 0; i< sizeof(nvc0_grhub_data) /4; i++){
          NV_WR32(0x4091c4,((uint32_t *)nvc0_grhub_data)[i]);
        }

        NV_WR32(0x409180,0x01000000);
        for (i = 0; i< sizeof(nvc0_grhub_code) /4; i++){
          if ((i & 0x3f) == 0){
            NV_WR32(0x409188, i>>6);
            }
        //NV_WR32(NVC0_CTXCTL_FUC_CODE_DATA, x);
          NV_WR32(0x409184,((uint32_t *)nvc0_grhub_code)[i]);
        }
#endif
        printf("HUB_code load finish\n");

	/* load GPC microcode. */
	NV_WR32(0x41a1c0, 0x01000000);
	for (i = 0; i < sizeof(nvc0_grgpc_data) / 4; i++)
		NV_WR32(0x41a1c4, ((uint32_t *)nvc0_grgpc_data)[i]);
	
	NV_WR32(0x41a180, 0x01000000);
	for (i = 0; i < sizeof(nvc0_grgpc_code) / 4; i++) {
		if ((i & 0x3f) == 0)
			NV_WR32(0x41a188, i >> 6);
		NV_WR32(0x41a184, ((uint32_t *)nvc0_grgpc_code)[i]);
	}
        printf("NVIDIA GPC code uploaded!\n");

	NV_WR32(0x260, val260);
}

/* @size must be aligned with 256 bytes. */
void upload_code (uint8_t *code, int size, uint32_t base) {
    int i;
    uint32_t val260 = NV_RD32(0x260);

    NV_WR32(0x260, val260 & ~1);
    uint32_t x = 0;
    //NV_WR32(NVC0_CTXCTL_FUC_CODE_INDEX, 0x01000000);
    NV_WR32(base + 0x180, 0x01000000);    
    for (i = 0; i < size; i += 4) {
	if (i % 256 == 0) {
	    //NV_WR32(NVC0_CTXCTL_FUC_CODE_SECTION, i / 256);
	    NV_WR32(base + 0x188, i / 256);
           }
	 x =
	    code[i] | code[i+1] << 8 | code[i+2] << 16 | code[i+3] << 24;
             
            //NV_WR32(NVC0_CTXCTL_FUC_CODE_DATA, x);
        NV_WR32(base + 0x184, x);	
    }
    for (; i < size; i += 4) {
	if (i % 256 == 0) {
	    //NV_WR32(NVC0_CTXCTL_FUC_CODE_SECTION, i / 256);
	    NV_WR32(base + 0x188, i / 256);
           }
	 x=0;
            //NV_WR32(NVC0_CTXCTL_FUC_CODE_DATA, x);
        NV_WR32(base + 0x184, x);	
    }
    NV_WR32(0x260, val260);
}

/* @size must be aligned with 256 bytes. */
void upload_data (uint8_t *data, int size, uint32_t base) {
    int i;
    uint32_t val260 = NV_RD32(0x260);
    
    NV_WR32(0x260, val260 & ~1);
    
    NV_WR32(base + 0x1c0, 0x01000000);
    for (i = 0; i < size; i += 4) {
	uint32_t x =
	    data[i] | data[i+1] << 8 | data[i+2] << 16 | data[i+3] << 24;
	NV_WR32(base + 0x1c4, x);
    }
    
    NV_WR32(0x260, val260);
}

void nvc0_debugunit(int base){
     printf("PGRAPH: %06x - done 0x%08x wrval 0x%08x ctrl 0x%08x\n",base,NV_RD32(base + 0x400),NV_RD32(base + 0x730),NV_RD32(base + 0x728));;
     printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x800), NV_RD32(base + 0x804),NV_RD32(base + 0x808),NV_RD32(base + 0x80c));
     printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x810), NV_RD32(base + 0x814),NV_RD32(base + 0x818),NV_RD32(base + 0x81c));
     printf("PGRAPH: %06x - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x68), NV_RD32(base + 0x64),NV_RD32(base + 0x500),NV_RD32(base + 0x504));
#if 0
     printf("PGRAPH: %06x - b00= 0x%08x a00 = 0x%08x cmd=0x%04x data=0x%08x\n",base,NV_RD32(base + 0xb00), NV_RD32(base + 0xa00),NV_RD32(base + 0x068),NV_RD32(base + 0x064));
     printf("PGRAPH: %06x - BASE= 0x%08x MCHAN= 0x%08x MCMD=0x%04x MTARGET= 0x%08x\n",base,NV_RD32(base + 0xa04), NV_RD32(base + 0xa0c),NV_RD32(base + 0xa10),NV_RD32(base + 0xa20));
     printf("PGRAPH: %06x - STR CMD= 0x%08x GENECNT= 0x%08x FSTGENE=0x%04x STRANDS0x%08x\n",base,NV_RD32(base + 0x928), NV_RD32(base + 0x918),NV_RD32(base + 0x91c),NV_RD32(base + 0x880));
     printf("PGRAPH: %06x - STR SVBASE= 0x%08x LDBASE= 0x%08x SIZE=0x%04x\n",base,NV_RD32(base + 0x908), NV_RD32(base + 0x90c),NV_RD32(base + 0x910));
     /* xfer  */
     printf("PGRAPH: %06x - xfer stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x110), NV_RD32(base + 0x114),NV_RD32(base + 0x118),NV_RD32(base + 0x11c));
     printf("PGRAPH: %06x - xfer stat 0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x120), NV_RD32(base + 0xa20),NV_RD32(base + 0x10c),0x0);
     printf("PGRAPH: %06x - RED  0x%08x 0x%08x 0x%08x 0x%08x\n",base,NV_RD32(base + 0x614), NV_RD32(base + 0xa20),NV_RD32(base + 0x10c),0x0);
#endif
}

void nvc0_debug(){
    printf("\n");
nvc0_debugunit(0x409000);
#if 0
nvc0_debugunit(0x502000);
nvc0_debugunit(0x50a000);
nvc0_debugunit(0x512000);
nvc0_debugunit(0x51a000);
#endif

#if 0
  /*debug message 409000 */
     printf("PGRAPH: 409000 - done 0x%08x wrval 0x%08x ctrl 0x%08x\n",NV_RD32(0x409400),NV_RD32(0x409730),NV_RD32(0x409728));;
     printf("PGRAPH: 409000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x409800), NV_RD32(0x409804),NV_RD32(0x409808),NV_RD32(0x40980c));
     printf("PGRAPH: 409000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x409810), NV_RD32(0x409814),NV_RD32(0x409818),NV_RD32(0x40981c));
     printf("PGRAPH: 409 - b00= 0x%08x a00 = 0x%08x cmd=0x%04x data=0x%08x\n",NV_RD32(0x409b00), NV_RD32(0x409a00),NV_RD32(0x409068),NV_RD32(0x409064));
     printf("PGRAPH: MEM: BASE= 0x%08x MCHAN= 0x%08x MCMD=0x%04x MTARGET= 0x%08x\n",NV_RD32(0x409a04), NV_RD32(0x409a0c),NV_RD32(0x409a10),NV_RD32(0x409a20));
     printf("PGRAPH: STRAND CMD= 0x%08x GENECNT= 0x%08x FSTGENE=0x%04x STRANDS0x%08x\n",NV_RD32(0x409928), NV_RD32(0x409918),NV_RD32(0x40991c),NV_RD32(0x409880));
    printf("PGRAPH: STRAND SVSWBASE= 0x%08x LDSWBASE= 0x%08x SIZE=0x%04x\n",NV_RD32(0x409908), NV_RD32(0x40990c),NV_RD32(0x409910));

#if 1
  /*debug message gpc1 502000*/
     printf("PGRAPH: 502000 - done 0x%08x cmd  0x%08x data 0x%08x \n",NV_RD32(0x502400),NV_RD32(0x502068),NV_RD32(0x502064));
     printf("PGRAPH: 502000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x502800), NV_RD32(0x502804),NV_RD32(0x502808),NV_RD32(0x50280c));
     printf("PGRAPH: 502000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x502810), NV_RD32(0x502814),NV_RD32(0x502818),NV_RD32(0x50281c));
     printf("PGRAPH: STRAND CMD= 0x%08x GENECNT= 0x%08x FSTGENE=0x%04x STRANDS0x%08x\n",NV_RD32(0x502928), NV_RD32(0x502918),NV_RD32(0x50291c),NV_RD32(0x502880));
    printf("PGRAPH: STRAND SVSWBASE= 0x%08x LDSWBASE= 0x%08x SIZE=0x%04x\n",NV_RD32(0x502908), NV_RD32(0x50290c),NV_RD32(0x502910));

  /*debug message gpc2 50a000*/
     printf("PGRAPH: 50a000 - done 0x%08x cmd  0x%08x data 0x%08x \n",NV_RD32(0x50a400),NV_RD32(0x50a068),NV_RD32(0x50a064));
     printf("PGRAPH: 50a000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x50a800), NV_RD32(0x50a804),NV_RD32(0x50a808),NV_RD32(0x50a80c));
     printf("PGRAPH: 50a000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x50a810), NV_RD32(0x50a814),NV_RD32(0x50a818),NV_RD32(0x50a81c));

  /*debug message gpc3 512000*/
     printf("PGRAPH: 502000 - done 0x%08x cmd  0x%08x data 0x%08x \n",NV_RD32(0x512400),NV_RD32(0x512068),NV_RD32(0x512064));
     printf("PGRAPH: 512000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x512800), NV_RD32(0x512804),NV_RD32(0x512808),NV_RD32(0x51280c));
     printf("PGRAPH: 512000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x512810), NV_RD32(0x512814),NV_RD32(0x512818),NV_RD32(0x51281c));

  /*debug message gpc4 51a000*/
     printf("PGRAPH: 502000 - done 0x%08x cmd  0x%08x data 0x%08x \n",NV_RD32(0x51a400),NV_RD32(0x51a068),NV_RD32(0x51a064));
     printf("PGRAPH: 51a000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x51a800), NV_RD32(0x51a804),NV_RD32(0x51a808),NV_RD32(0x51a80c));
     printf("PGRAPH: 51a000 - stat 0x%08x 0x%08x 0x%08x 0x%08x\n",NV_RD32(0x51a810), NV_RD32(0x51a814),NV_RD32(0x51a818),NV_RD32(0x51a81c));
#endif

#endif
}


int NV_WAIT(uint32_t addr, uint32_t timeout, uint32_t val)
{
    uint32_t x;
    do {
      if(!(timeout % 0x50000))
      {
        printf("timeout:0x%08x\n",timeout);
        nvc0_debug();

      }
	x = NV_RD32(addr);
	if (val == x) {
	    return 1;
	}
    } while (timeout--);
    return 0;
}

int NV_WAIT_NEQ(uint32_t addr, uint32_t timeout, uint32_t mask, uint32_t val)
{
  uint32_t x;
  do {
    x = NV_RD32(addr) & mask;
    if (val != x){
      return 1;
    }
  }while(timeout--);
  return 0;
}



int dev_store_ctx(){
  printf("PGRAPH: Start store ctx\n");
  printf("PGRAPH: MEMBASE: %0x\n",mmio_read09(0xa04));

  uint32_t inst;
  inst = mmio_read09(0xb00) & 0xfffffff;
  nvc0_debug();

  mmio_write09(CC_SCRATCH_CLEAR0,0x3);
  mmio_write09(WRDATA,(0x8<<28) | inst);
  mmio_write09(WRCMD,0x9);
printf("inst = %0x",(0x8<<28) | inst);
  if(!mmio_wait09(FUC_REG_RETURN,0x1)){
    printf("failed to store context\n");
  printf("PGRAPH: MEMBASE: %0x\n",mmio_read09(0xa04));
    nvc0_debug();
    return 0;
  }
  printf("success! to store context\n");
  printf("PGRAPH: MEMBASE: %0x\n",mmio_read09(0xa04));
  nvc0_debug();
 return 1;
}

int dev_load_ctx(){
  printf("PGRAPH: Start load ctx\n");
  printf("PGRAPH: MEMBASE: %0x\n",mmio_read09(0xa04));
  uint32_t inst=0x80020ad7;
  nvc0_debug();

  mmio_write09(0x614,0x070);
  mmio_write09(0x614,0x770);
  NV_WR32(0x40802c,1);
  mmio_write09(0x840,0x30);

  nvc0_debug();

  mmio_write09(WRDATA, inst);
  mmio_write09(WRCMD,0x3);

  printf("a20= 0x%x\n",mmio_read09(0xa20));
  printf("success! to load context\n");
  printf("a20= 0x%x\n",mmio_read09(0xa20));
  nvc0_debug();
  printf("a20= 0x%x\n",mmio_read09(0xa20));
  
  printf("PGRAPH: MEMBASE: %0x\n",mmio_read09(0xa04));
  return 1;
}


void dev_start_microcode(){

int i,j;
  printf("PGHRAPH: initializing ....\n");
  nvc0_debug();
 if (!mmio_wait09(FUC_REG_RETURN, 1)) {
  printf("PGRAPH: HUB_INIT/GPC_INIT timed out\n");
  nvc0_debug();
  return;
 }
 printf("DB_PGRAPH: END HUB_INIT/GPC_INIT\nDB_PGRAPH: START CMD 0x21\n");
 nvc0_debug();
 mmio_write09(CC_SCRATCH_CLEAR0,0xffffffff);
 mmio_write09(WRDATA,0x7fffffff);
 mmio_write09(WRCMD,0x21);
  
 printf("DB_PGRAPH: END CMD 0x21\nDB_PGRAPH: START CMD 0x10 \n");
 nvc0_debug();
 
 printf("FUC_ST=0x%x\n", mmio_read09(0x910));


 mmio_write09(CC_SCRATCH_CLEAR0,0xffffffff);
 mmio_write09(WRDATA,0x0);
 mmio_write09(WRCMD,0x10); /*grctx size request*/
 
 printf("SEND END CMD 0x10\n");
 nvc0_debug();

 printf("FUC_ST=0x%x\n", mmio_read09(0x910));
 nvc0_debug();
 if(!mmio_wait09_neq(FUC_REG_RETURN,~0,0x0)){
  printf("PGRAPH: GRCTX_SIZE_TIMEOUT\n");
   nvc0_debug();
  return;
 }
 printf("GRCTX_SIZE_REQUEST FINISH!!!!\nSTART CMD 0x16\n");
 
 nvc0_debug();
 nvc0_debug();
 mmio_write09(CC_SCRATCH_CLEAR0,0xffffffff);
 mmio_write09(WRDATA,0x0);
 mmio_write09(WRCMD,0x16); 

 if(!mmio_wait09_neq(FUC_REG_RETURN,~0,0x0)){
  printf("PGRAPH: CMD 0x16 timed out\n");
   nvc0_debug();
  return;
 }
 printf("CMD0x16 FINISH!!!!\nSTART CMD 0x25\n");
 
 nvc0_debug();
 mmio_write09(CC_SCRATCH_CLEAR0,0xffffffff);
 mmio_write09(WRDATA,0x0);
 mmio_write09(WRCMD,0x25); 

 if(!mmio_wait09_neq(FUC_REG_RETURN,~0,0x0)){
  printf("PGRAPH: CMD 0x25 timed out\n");
   nvc0_debug();
  return;
 }
/*gpc ~~~~ write here*/
  printf("PGRAPH: End CMD0x25 \n"); 
 nvc0_debug();
  uint32_t cx_num,temp;
  cx_num = mmio_read09(0x880);
  for (i=0; i< cx_num; ++i){
    mmio_write09(0xffc,i);
    temp=mmio_read09(0x910);
  }

  
  printf("PGRAPH: cx_num= 0x%x\n",cx_num); 
   nvc0_debug();
  cx_num = NV_RD32(0x500000+0x02880);
  for(i=0;i<4;++i){//graph->gpc_count;++i){
    for(j=0;j<cx_num;++j){
      NV_WR32(0x502ffc + 0x8000*i,j);
      temp=NV_RD32(0x502910 + 0x8000*i);
      printf("t=%x ,",temp);
    }
    printf("\n");
  }
  printf("initialize complete....:%d\n",temp);
  nvc0_debug();
#if 1
  dev_load_ctx();
  if(!dev_store_ctx()){
    printf("Code failed\n");
  }
#endif
  return;


}

void doit(void)
{
    char buff[256];
    int command,data;

   mmio_write09(0x840, 0xffffffff);
    /* pull trigger! */
    mmio_write1a(0x10c, 0); /* what is this? */
    mmio_write09(0x10c, 0); /* what is this? */
    //mmio_write1a(0x104, 0); /* entry point */
    //mmio_write09(0x104, 0); /* entry point */
    mmio_write1a(0x100, 2); /* trigger */
    mmio_write09(0x100, 2); /* trigger */
    
 //   while (!mmio_read09(0x008));
    
    
    /* wait for the fuc to complete executions. */
    
  /* yukke added code 120510 */
  while(1){
      /* input command */
      printf(">command :  0x");
      fgets(buff,COM_SIZE,stdin);
      command = strtol(buff,NULL,16);

      if(command != 0xff ){ 
       if(command == 0xffff){
         printf("in device driver mode.\n");
         dev_start_microcode();
         return; 
       }
      printf(">data    :  0x");
      fgets(buff,COM_SIZE,stdin);
      data = strtol(buff,NULL,16);
      mmio_write09(WRDATA,data); //write data
      mmio_write09(WRCMD,command); //write command
      printf("Send 0x%x command, 0x%x data\n",command,data);
     }
    nvc0_debug();
     // if(NV_RD32(0x409000+FUC_REG_RETURN) != 1 ){ //wait complete execute
      //   printf("executed!\n");
      // break;
    // }
   // }
 // if (!mmio_wait09(FUC_REG_RETURN, 1)) {
  //	printf("Microcontroller hung up...\n");
   //   }
  }/* while */  

}

int read_binfile(char* filename, uint8_t *buf, int max_size)
{
    int size = 0;
    FILE* fp;
    
    if ((fp = fopen(filename, "rb")) == NULL) {
	printf("Failed to open %s\n", filename);
	return 0;
    }

    /* check the file size. */
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    if (size > max_size) {
	goto out;
    }
    
    /* read the file. */
    fseek(fp, 0, SEEK_SET);
    fread(buf, sizeof(char), size, fp);

  out:
    fclose(fp);

    return size;
}

int init_device(void)
{
    /* init PCI. */
    init();

    /* reset engine. */
    NV_WR32(0x200, 0xfffd3ffd);
    NV_WR32(0x200, 0xffffffff);
    NV_WR32(0x200, NV_RD32(0x200) & 0xffffefff);
    NV_WR32(0x200, NV_RD32(0x200) | 0x00001000);
    sleep(1);

    return TRUE;
}

void fini_device(void)
{
    fini();
}

int main(int argc, char **argv)
{

    char codefile09[256] = "";
    char datafile09[256] = "";
    char codefile1a[256] = "";
    char datafile1a[256] = "";
    uint8_t code[MAX_CODE_SIZE] = {0};
    uint8_t data[MAX_DATA_SIZE] = {0};
    int size;
    int cs_size, ds_size;
    int i;
    
    fini_device();
  
    for (i = 0; i < argc; i++) {
	if (strcmp(argv[i], "-09c") == 0) {
	    i++;
	    if (i < argc) {
		strcpy(codefile09, argv[i]);
	    }
	    else {
		printf("No code file specified\n");
		return 0;
	    }
	}
	else if (strcmp(argv[i], "-09d") == 0) {
	    i++;
	    if (i < argc) {
		strcpy(datafile09, argv[i]);
	    }
	    else {
		printf("No data file specified\n");
		return 0;
	    }
	}
	else if (strcmp(argv[i], "-1ac") == 0) {
	    i++;
	    if (i < argc) {
		strcpy(codefile1a, argv[i]);
	    }
	    else {
		printf("No code file specified\n");
		return 0;
	    }
	}
	else if (strcmp(argv[i], "-1ad") == 0) {
	    i++;
	    if (i < argc) {
		strcpy(datafile1a, argv[i]);
	    }
	    else {
		printf("No data file specified\n");
		return 0;
	    }
	}
	else if (i > 0) {
	    printf("Invalid arguments\n");
	    return 0;
	}
    }


    
    printf("Code09 file: %s\n", codefile09);
    printf("Data09 file: %s\n", datafile09);    

 
    /* init the device. you can't access the fuc until you do this. */
    init_device();

    /****************************
     *         Code09           *
     ****************************/
    /* check the code and data section size. */
    cs_size = (mmio_read09(0x620) & 0xff) << 8;
    printf("Code09 section: 0x%x bytes\n", cs_size);
    ds_size = ((mmio_read09(0x620) >> 8) & 0xff) << 8;
    printf("Data09 section: 0x%x bytes\n", ds_size);

#ifndef USE_BLOB_UCODE
    /* read the code file. */
    size = read_binfile(codefile09, code, MAX_CODE_SIZE);
    printf("Code09 size: 0x%x bytes\n", size);
    if (size > cs_size) {
	printf("Code09 cannot fit the code section\n");
	goto out;
    }
    /* upload the code. */
    upload_code(code, cs_size, NVC0_CTXCTL_FUC_BASE09);
    printf("Code09 uploaded\n");


    /* read the data file. */
    size = read_binfile(datafile09, data, MAX_DATA_SIZE);
    printf("Data size: 0x%x bytes\n", size);    
    if (size > ds_size) {
	printf("Data09 cannot fit the data section\n");
	goto out;
    }
    /* upload the data. */
    upload_data(data, ds_size, NVC0_CTXCTL_FUC_BASE09);
    printf("Data09 uploaded\n\n");
#endif

    /****************************
     *         Code1A           *
     ****************************/

#if 1
    /* check the code and data section size. */
    cs_size = (mmio_read1a(0x620) & 0xff) << 8;
    printf("Code1A section: 0x%x bytes\n", cs_size);
    ds_size = ((mmio_read1a(0x620) >> 8) & 0xff) << 8;
    printf("Data1A section: 0x%x bytes\n", ds_size);

    /* read the code file. */
    size = read_binfile(codefile1a, code, MAX_CODE_SIZE);
    printf("Code1A size: 0x%x bytes\n", size);
    if (size > cs_size) {
	printf("Code1A cannot fit the code section\n");
	goto out;
    }
    /* upload the code1a. */
    printf("Code1A uploaded\n");
    upload_code(code, cs_size, NVC0_CTXCTL_FUC_BASE1A);
    printf("Code1A uploaded\n");

    /* read the data file. */
    size = read_binfile(datafile1a, data, MAX_DATA_SIZE);
    printf("Data size: 0x%x bytes\n", size);    
    if (size > ds_size) {
	printf("Data1A cannot fit the data section\n");
	goto out;
    }
    /* upload the data. */
    upload_data(data, ds_size, NVC0_CTXCTL_FUC_BASE1A);
    printf("Data1A uploaded\n");
#else
    /* upload code & data nvc0_ctxctl.h */
    nvc0_graph_load_microcode();
#endif
    /* let the microcontroller execute the firmware. */
    doit();
    printf("Microcontroller executed\n");


#if 0 // test code...
    printf("Waiting for 0x10\n");
    mmio_write09(0x840, 0xffffffff);
    mmio_write09(0x500, 0);
    mmio_write09(0x504, 0x10);
    
    if (!mmio_wait09(0x800, 0))
	printf("PGRAPH: 0x9800 stalled\n");

#endif
    
printf("Return value = 0x%x\n", mmio_read09(FUC_REG_RETURN));


  out:
    /* finish the device. */
    fini_device();

    return 0;
}
