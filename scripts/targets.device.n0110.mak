$(BUILD_DIR)/test_external_flash.$(EXE): LDSCRIPT = ion/test/external_flash_tests.ld
$(BUILD_DIR)/test_external_flash.$(EXE): $(BUILD_DIR)/quiz/src/external_flash_tests_symbols.o $(runner_objs) $(call object_for,$(tests_external_flash))
