executables += test.external_flash.write test.external_flash.read

$(BUILD_DIR)/test.external_flash.%.$(EXE): LDSCRIPT = ion/test/device/n0110/external_flash_tests.ld
test_external_flash_src = $(src) $(ion_device_dfu_relogated_src) $(runner_src)
$(BUILD_DIR)/test.external_flash.read.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_read_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_read_src))
$(BUILD_DIR)/test.external_flash.write.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_write_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_write_src))
