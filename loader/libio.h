extern void* regs;
extern void* fb;
extern void* ramin;

#define NV_RD32(reg) (reg!=0x409504?*(volatile uint32_t*)((char*)(regs)+(reg)):0)
#define NV_RD08(reg) (*(volatile uint8_t*)((char*)(regs)+(reg)))
#define NV_WR32(reg, val) do{                 \
      *(volatile uint32_t*)((char*)(regs)+(reg)) = (val);  \
   }while(0)
#define NV_WR08(reg, val) do{                 \
      *(volatile uint8_t*)((char*)(regs)+(reg)) = (val);   \
   }while(0)

#define NV_FRD32(reg) (*(volatile uint32_t*)((char*)(fb)+(reg)))
#define NV_FRD08(reg) (*(volatile uint8_t*)((char*)(fb)+(reg)))
#define NV_FWR32(reg, val) do{                 \
      *(volatile uint32_t*)((char*)(fb)+(reg)) = (val);  \
   }while(0)
#define NV_FWR08(reg, val) do{                 \
      *(volatile uint8_t*)((char*)(fb)+(reg)) = (val);   \
   }while(0)

#define NV_RRD32(reg) (*(volatile uint32_t*)((char*)(ramin)+(reg)))
#define NV_RRD08(reg) (*(volatile uint8_t*)((char*)(ramin)+(reg)))
#define NV_RWR32(reg, val) do{                 \
      *(volatile uint32_t*)((char*)(ramin)+(reg)) = (val);  \
   }while(0)
#define NV_RWR08(reg, val) do{                 \
      *(volatile uint8_t*)((char*)(ramin)+(reg)) = (val);   \
   }while(0)

void init();
void fini();
