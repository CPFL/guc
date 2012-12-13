TARGET = test_intr

# COMPILER
CLANG = clang
CLANG_FLAGS = -m32 -emit-llvm -S -O2
ENVYAS = envyas
ENVYAS_FLAGS = -m fuc -V fuc3
LLC = llc
LLC_FLAGS = -march=nvfc

# DIR
NVFC_ROOT = ../../
SCRIPT_DIR = $(NVFC_ROOT)/script

# SCRIPT
LLVM2ENVYAS = $(SCRIPT_DIR)/llvm2envyas
HEX2BYTELIST = $(SCRIPT_DIR)/hex2bytelist
HEX2BIN = $(SCRIPT_DIR)/hex2bin

# LOADER
LOADER = $(NVFC_ROOT)/loader_firmware/fucload

CAT = cat


all: main.c
	$(CLANG) $< -o $(TARGET).bc $(CLANG_FLAGS)
	$(LLC) $(TARGET).bc -o $(TARGET).s $(LLC_FLAGS)
	$(LLVM2ENVYAS) $(TARGET).s -c $(TARGET).tmp -d $(TARGET).data.S
	$(CAT) boot.S $(TARGET).tmp > $(TARGET).S
	$(ENVYAS) -i $(TARGET).S $(ENVYAS_FLAGS) > $(TARGET).bin
	$(HEX2BIN) $(TARGET).data.S $(TARGET).data.bin

run:
	sudo $(LOADER) -09c $(TARGET).bin -09d $(TARGET).data.bin -1ac $(TARGET).bin -1ad $(TARGET).data.bin
clean:
	rm -f $(TARGET).* *~ 
