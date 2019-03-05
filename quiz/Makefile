SFLAGS += -Iquiz/include

QUIZ_USE_CONSOLE ?= 0
$(call object_for,quiz/src/runner.cpp): SFLAGS += -DQUIZ_USE_CONSOLE=$(QUIZ_USE_CONSOLE)

symbols_file = $(BUILD_DIR)/quiz/src/symbols.c

$(symbols_file): $(tests)
	@echo "AWK     $@"
	$(Q) awk -f quiz/src/symbols.awk $(tests) > $@

runner_src += $(addprefix quiz/src/, \
  assertions.cpp \
  i18n.cpp \
  runner.cpp \
)

runner_src += $(symbols_file)

runner_objs = $(call object_for,$(runner_src))
$(runner_objs): SFLAGS += -Iquiz/src

test.$(EXE): $(runner_objs) $(call object_for,$(tests))
