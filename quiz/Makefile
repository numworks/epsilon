SFLAGS += -Iquiz/include

define rule_for_quiz_symbols
$$(BUILD_DIR)/quiz/src/$(subst _src,,$(1))_symbols.c: $$($(1)) | $$$$(@D)/.
	@ echo "AWK     $$@"
	$$(Q) awk -f quiz/src/symbols.awk $$($(1)) > $$@
endef

$(eval $(call rule_for_quiz_symbols,tests_src))
$(eval $(call rule_for_quiz_symbols,test_ion_external_flash_write_src))
$(eval $(call rule_for_quiz_symbols,test_ion_external_flash_read_src))

runner_src += $(addprefix quiz/src/, \
  assertions.cpp \
  i18n.cpp \
  runner.cpp \
  stopwatch.cpp \
)

runner_src += $(BUILD_DIR)/quiz/src/tests_symbols.c

$(call object_for,quiz/src/i18n.cpp): $(BUILD_DIR)/apps/i18n.h

$(call object_for,$(runner_src)): SFLAGS += -Iquiz/src
$(BUILD_DIR)/quiz/src/%_symbols.o: SFLAGS += -Iquiz/src
