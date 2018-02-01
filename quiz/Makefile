SFLAGS += -Iquiz/include

QUIZ_USE_CONSOLE ?= 0
quiz/src/runner.o: SFLAGS += -DQUIZ_USE_CONSOLE=$(QUIZ_USE_CONSOLE)

symbols_file = $(addprefix quiz/src/, symbols.c)
products += $(symbols_file)

$(symbols_file): $(tests)
	@echo "AWK     $@"
	$(Q) awk -f quiz/src/symbols.awk $(tests) > $@

runner_objs += $(addprefix quiz/src/, runner.o symbols.o i18n.o)
test_objs += $(subst .c,.o, $(subst .cpp,.o,$(tests)))

test.$(EXE): $(runner_objs) $(test_objs)

products += test.$(EXE) $(runner_objs) $(test_objs)
