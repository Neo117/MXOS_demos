# Date    : 2019/08/16
# Author  : guidx
# Mail    : guidx@mxchip.com
USER_BIN_DIR=.\mxos\platform\MCU\mx1400\image

JSON_DIR=.\mxos\platform\MCU\mx1400\user_json

ELF2BIN = .\mxos\platform\MCU\mx1400\elf2bin.exe

EXTRA_POST_BUILD_TARGETS += $(MXOS_ALL_BIN_OUTPUT_FILE) Image

Image: 
	cp $(LINK_OUTPUT_FILE) $(USER_BIN_DIR)/application_is.axf
	cp $(JSON_DIR)\partition.json ./
ifeq ($(findstring Linux, $(OS)), Linux)
	chmod 0774 $(ELF2BIN)
endif
	$(ELF2BIN) keygen  $(JSON_DIR)\keycfg.json
	$(ELF2BIN) convert $(JSON_DIR)\amebaz2_bootloader.json BOOTLOADER secure_bit=0
	$(ELF2BIN) convert $(JSON_DIR)\amebaz2_bootloader.json PARTITIONTABLE secure_bit=0
	$(ELF2BIN) convert $(JSON_DIR)\amebaz2_firmware_is.json FIRMWARE secure_bit=0
	$(ELF2BIN) combine $(USER_BIN_DIR)\all.bin PTAB=$(USER_BIN_DIR)\partition.bin,BOOT=$(USER_BIN_DIR)\bootloader.bin,FW1=$(USER_BIN_DIR)\firmware_is.bin,FW2=$(USER_BIN_DIR)\ate.bin
	
	cp $(USER_BIN_DIR)\firmware_is.bin $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).bin
	cp $(USER_BIN_DIR)\firmware_is.bin $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).ota.bin
	cp $(USER_BIN_DIR)\all.bin $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).all.bin
	
	$(PYTHON) $(MXOS_OS_PATH)/makefiles/scripts/add_md5.py $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).ota.bin

	$(RM) ./partition.json
	$(RM) ./key.json
	$(RM) $(USER_BIN_DIR)\all.bin
	$(RM) $(USER_BIN_DIR)\firmware_is.bin
	$(RM) $(USER_BIN_DIR)\partition.bin
