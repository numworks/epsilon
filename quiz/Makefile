SFLAGS += -Iquiz/include

symbols_file = $(addprefix quiz/src/, symbols.c)
products += $(symbols_file)

$(symbols_file): $(tests)
	@echo "AWK     $@"
	@awk -f quiz/src/symbols.awk $(tests) > $@

runner_objs += $(addprefix quiz/src/, runner.o symbols.o)
test_objs = $(subst .c,.o, $(subst .cpp,.o,$(tests)))

test.elf: $(objs) $(runner_objs) $(test_objs)
	@echo "LD      $@"
	@$(LD) $(LDFLAGS) $(objs) $(test_objs) -o $@
