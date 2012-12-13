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

#define MAX_CODE_SIZE     0x10000
#define MAX_DATA_SIZE     0x10000

#define FALSE 0
#define TRUE 1

#define mmio_read09(addr) \
    NV_RD32(NVC0_CTXCTL_FUC_BASE09 + (addr))
#define mmio_write09(addr, val)				\
    NV_WR32(NVC0_CTXCTL_FUC_BASE09 + (addr), (val))
#define mmio_wait09(addr, val)				\
    NV_WAIT(NVC0_CTXCTL_FUC_BASE09 + (addr), ~0, (val))

#define mmio_read1a(addr) \
    NV_RD32(NVC0_CTXCTL_FUC_BASE1A + (addr))
#define mmio_write1a(addr, val)				\
    NV_WR32(NVC0_CTXCTL_FUC_BASE1A + (addr), (val))
#define mmio_wait1a(addr, val)				\
    NV_WAIT(NVC0_CTXCTL_FUC_BASE1A + (addr), ~0, (val))

/* @size must be aligned with 256 bytes. */
void upload_code (uint8_t *code, int size, uint32_t base) {
    int i;
    uint32_t val260 = NV_RD32(0x260);

    NV_WR32(0x260, val260 & ~1);
	
    //NV_WR32(NVC0_CTXCTL_FUC_CODE_INDEX, 0x01000000);
    NV_WR32(base + 0x180, 0x01000000);    
    for (i = 0; i < size; i += 4) {
	if (i % 256 == 0) {
	    //NV_WR32(NVC0_CTXCTL_FUC_CODE_SECTION, i / 256);
	    NV_WR32(base + 0x188, i / 256);
	}
	uint32_t x =
	    code[i] | code[i+1] << 8 | code[i+2] << 16 | code[i+3] << 24;
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

int NV_WAIT(uint32_t addr, uint32_t timeout, uint32_t val)
{
    uint32_t x;
    do {
	x = NV_RD32(addr);
	if (val == x) {
	    return 1;
	}
    } while (timeout--);
    return 0;
}

void doit(void)
{
    mmio_write09(0x840, 0xffffffff);
    /* pull trigger! */
    mmio_write1a(0x10c, 0); /* what is this? */
    mmio_write09(0x10c, 0); /* what is this? */
    //mmio_write1a(0x104, 0); /* entry point */
    //mmio_write09(0x104, 0); /* entry point */
    mmio_write1a(0x100, 2); /* trigger */
    mmio_write09(0x100, 2); /* trigger */
    //while (!mmio_read09(0x008));

    /* wait for the fuc to complete executions. */
   // if (!mmio_wait09(FUC_REG_RETURN, 1)) {
//	printf("Microcontroller hung up...\n");
 //   }
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

    /****************************
     *         Code1A           *
     ****************************/

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
    
    /* let the microcontroller execute the firmware. */
    doit();
    printf("Microcontroller executed\n");

#if 1 // test code...
    printf("Waiting for 0x10\n");
    mmio_write09(0x840, 0xffffffff);
    mmio_write09(0x500, 0xfffff);
    mmio_write09(0x504, 0x10);
    
//    if (!mmio_wait09(0x800, 0))
//	printf("PGRAPH: 0x9800 stalled\n");

#endif
    
    printf("Return value = 0x%x\n", mmio_read09(FUC_REG_RETURN));
    printf("09:Reg1 = %d\n", mmio_read09(FUC_REG_SCRATCH1));
    printf("09:Reg2 = 0x%x\n", mmio_read09(FUC_REG_SCRATCH2));
    printf("1A:Reg1 = %d\n", mmio_read1a(FUC_REG_SCRATCH1));
    printf("1A:Reg2 = 0x%x\n", mmio_read1a(FUC_REG_SCRATCH2));
    
    printf("09:NEWSCRATCH: 1: 0x%08x, 2: 0x%08x, 3: 0x%08x, 4: 0x%08x \n", mmio_read09(0x800)
	    ,mmio_read09(0x804),mmio_read09(0x808),mmio_read09(0x80c));
    printf("09:NEWSCRATCH: 5: 0x%08x, 6: 0x%08x, 7: 0x%08x, 8: 0x%08x \n", mmio_read09(0x810)
	    ,mmio_read09(0x814),mmio_read09(0x818),mmio_read09(0x81c));
out:
    /* finish the device. */
    fini_device();

    return 0;
}
