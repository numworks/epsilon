SFLAGS += -Iquiz/include

QUIZ_USE_CONSOLE ?= 0
$(call object_for,quiz/src/runner.cpp): SFLAGS += -DQUIZ_USE_CONSOLE=$(QUIZ_USE_CONSOLE)

define rule_for_quiz_symbols
$$(BUILD_DIR)/quiz/src/$(1)_symbols.c: $$($(1)) | $$$$(@D)/.
	@ echo "AWK     $$@"
	$$(Q) awk -f quiz/src/symbols.awk $$($(1)) > $$@
endef

$(eval $(call rule_for_quiz_symbols,tests))
$(eval $(call rule_for_quiz_symbols,external_flash_tests))

runner_src += $(addprefix quiz/src/, \
  assertions.cpp \
  i18n.cpp \
  runner.cpp \
)

runner_src += $(symbols_file)

runner_objs = $(call object_for,$(runner_src))
$(runner_objs): SFLAGS += -Iquiz/src
$(BUILD_DIR)/quiz/src/%_symbols.o: SFLAGS += -Iquiz/src


$(BUILD_DIR)/test.$(EXE): $(BUILD_DIR)/quiz/src/tests_symbols.o $(runner_objs) $(call object_for,$(tests))
