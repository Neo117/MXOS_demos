EXTRA_POST_BUILD_TARGETS += postbuild

ADD_MD5_SCRIPT := $(SOURCE_ROOT)/mxos/platform/MCU/mx1410/scripts/add_md5.py

NM_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.nm)
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py
ALL_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.all$(BIN_OUTPUT_SUFFIX))
OTA_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX))
APP_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.app$(BIN_OUTPUT_SUFFIX))


postbuild: build_done
	$(NM) $(LINK_OUTPUT_FILE) | sort > $(NM_OUTPUT_FILE)
	$(OBJCOPY) -j .ram_image2.entry -j .ram_image2.text -j .ram_image2.data -Obinary $(STRIPPED_LINK_OUTPUT_FILE) $(OUTPUT_DIR)/binary/ram_2.bin
	$(OBJCOPY) -j .xip_image2.text -Obinary $(STRIPPED_LINK_OUTPUT_FILE) $(OUTPUT_DIR)/binary/xip_image2.bin
	$(PYTHON) mxos/platform/MCU/mx1410/scripts/prepend_header.py $(OUTPUT_DIR)/binary/ram_2.bin __ram_image2_text_start__ $(NM_OUTPUT_FILE)
	$(PYTHON) mxos/platform/MCU/mx1410/scripts/prepend_header.py $(OUTPUT_DIR)/binary/xip_image2.bin __flash_text_start__ $(NM_OUTPUT_FILE)
	$(CAT) $(call CONV_SLASHES,$(OUTPUT_DIR)/binary/xip_image2_prepend.bin) $(call CONV_SLASHES,$(OUTPUT_DIR)/binary/ram_2_prepend.bin) > $(call CONV_SLASHES,$(BIN_OUTPUT_FILE))
	$(PYTHON) mxos/platform/MCU/mx1410/scripts/pad.py $(BIN_OUTPUT_FILE)
	$(CAT) $(call CONV_SLASHES,$(SOURCE_ROOT)mxos/platform/MCU/mx1410/image/km0_image2_all.bin) $(call CONV_SLASHES,$(BIN_OUTPUT_FILE)) > $(call CONV_SLASHES,$(APP_BIN_OUTPUT_FILE))

	$(QUIET)$(RM) $(ALL_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(KM0_BOOT_OFFSET)   $(KM0_BOOT_FILE)              
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(SYSTEM_OFFSET)     $(SYSTEM_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(KM4_BOOT_OFFSET)   $(KM4_BOOT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(OTA_OFFSET)        $(APP_BIN_OUTPUT_FILE) 
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(ATE_OFFSET)        $(ATE_FILE) 

	$(PYTHON) mxos/platform/MCU/mx1410/scripts/remove_signature.py $(APP_BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE) 
	$(PYTHON) $(ADD_MD5_SCRIPT) $(OTA_BIN_OUTPUT_FILE)
	$(PYTHON) $(ADD_MD5_SCRIPT) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/eclipse_debug/)
	$(QUIET)$(CP) $(LINK_OUTPUT_FILE) $(BUILD_DIR)/eclipse_debug/last_built.elf
