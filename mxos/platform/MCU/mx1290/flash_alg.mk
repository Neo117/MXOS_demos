BOOT_BIN_FILE := $(MXOS_OS_PATH)/platform/MCU/mx1290/image/boot.bin
ATE_BIN_FILE := $(MXOS_OS_PATH)/platform/MCU/mx1290/image/ate.bin

ifneq (total,$(findstring total,$(MAKECMDGOALS)))
download: $(BIN_OUTPUT_FILE) display_map_summary
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading applcation, size: $(IMAGE_SIZE) bytes... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(BIN_OUTPUT_FILE) -a 0x18000 -m mx1290
else
download: $(BIN_OUTPUT_FILE) display_map_summary
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(BOOT_BIN_FILE)))
	$(QUIET)$(ECHO) Downloading bootloader, size: $(IMAGE_SIZE) bytes... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(BOOT_BIN_FILE) -a 0x00 -m mx1290
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading applcation, size: $(IMAGE_SIZE) bytes... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(BIN_OUTPUT_FILE) -a 0x18000 -m mx1290
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(ATE_BIN_FILE)))
	$(QUIET)$(ECHO) Downloading ate, size: $(IMAGE_SIZE) bytes... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(ATE_BIN_FILE) -a 0x100000 -m mx1290
endif

total:
	@: