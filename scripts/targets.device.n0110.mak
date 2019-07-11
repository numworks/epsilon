executables += test.external_flash

$(BUILD_DIR)/test.external_flash.$(EXE): LDSCRIPT = ion/test/device/n0110/external_flash_tests.ld
$(BUILD_DIR)/test.external_flash.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_symbols.o $(call object_for,$(src) $(ion_device_dfu_relocated_src) $(test_ion_external_flash_src) $(runner_src))
