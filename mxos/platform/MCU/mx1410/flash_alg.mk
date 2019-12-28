KM0_BOOT_FILE    := mxos/module/$(MODULE)/km0_boot_all.bin
KM0_BOOT_OFFSET  := 0x0

SYSTEM_FILE    := mxos/platform/MCU/mx1410/image/system.bin
SYSTEM_OFFSET  := 0x3000

KM4_BOOT_FILE    := mxos/platform/MCU/mx1410/image/km4_boot_all.bin
KM4_BOOT_OFFSET  := 0x4000

ATE_FILE    := mxos/platform/MCU/mx1410/image/ate.bin
ATE_OFFSET  := 0x188000

APP_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.app$(BIN_OUTPUT_SUFFIX))
OTA_OFFSET  := 0x6000

ifneq (total,$(findstring total,$(MAKECMDGOALS)))
download: postbuild
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(APP_BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading applcation, size: $(IMAGE_SIZE) bytes @$(OTA_OFFSET)... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(APP_BIN_OUTPUT_FILE) -a $(OTA_OFFSET) -m mx1410
else
download: postbuild
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(KM0_BOOT_FILE)))
	$(QUIET)$(ECHO) Downloading km0 boot, size: $(IMAGE_SIZE) bytes @$(KM0_BOOT_OFFSET)... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(KM0_BOOT_FILE) -a $(KM0_BOOT_OFFSET) -m mx1410
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(SYSTEM_FILE)))
	$(QUIET)$(ECHO) Downloading system, size: $(IMAGE_SIZE) bytes @$(SYSTEM_OFFSET)... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(SYSTEM_FILE) -a $(SYSTEM_OFFSET) -m mx1410
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(KM4_BOOT_FILE)))
	$(QUIET)$(ECHO) Downloading km4 boot, size: $(IMAGE_SIZE) bytes @$(KM4_BOOT_OFFSET)... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(KM4_BOOT_FILE) -a $(KM4_BOOT_OFFSET) -m mx1410
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(APP_BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading application, size: $(IMAGE_SIZE) bytes @$(OTA_OFFSET)... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(APP_BIN_OUTPUT_FILE) -a $(OTA_OFFSET) -m mx1410
	$(eval IMAGE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(ATE_FILE)))
	$(QUIET)$(ECHO) Downloading ate, size: $(IMAGE_SIZE) bytes @$(ATE_OFFSET)... 
	$(PYTHON) mxos/makefiles/scripts/flash_alg_progressbar/flash_alg_write.py -o $(OPENOCD_FULL_NAME) -d $(JTAG) -f $(ATE_FILE) -a $(ATE_OFFSET) -m mx1410
endif

total:
	@:
