# AMEBAZ2_GCCTOOLDIR	= .\mxos\platform\MCU\RTL8720C\gdb
# CROSS_COMPILE = $(TOOLS_ROOT)/compiler/arm-none-eabi-$(TOOLCHAIN_VERSION)/Win32/bin/arm-none-eabi-
# GDB = $(CROSS_COMPILE)gdb

# FIRMWARENAME = $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).all.bin
# FIRMWARE_SIZE = $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(FIRMWARENAME))

# FIRMWARE = (int)&FlashBufferStart-$$Offset $$ReadPtr $$ReadEndPtr

# $(info Resources: $(FIRMWARENAME))
# $(info Resources: $(FIRMWARE_SIZE))

# download: 
# 	echo set $$(FirmwareName) = "$(FIRMWARENAME)" > fwinfo.gdb
# 	echo set $$(FirmwareSize) = $(FIRMWARE_SIZE) >> fwinfo.gdb
# # echo "restore $(FIRMWARENAME) binary $(FIRMWARE)" > restore.gdb
# 	$(GDB) -x $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader_jlink.txt

