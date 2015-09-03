symbols_file = $(addprefix quiz/src/, symbols.c)
products += $(symbols_file)

$(symbols_file): $(tests)
	@echo "AWK     $@"
	@awk -f quiz/src/symbols.awk $(tests) > $@

test_objs = $(subst .c,.o, $(subst .cpp,.o,$(tests)))

test.elf: quiz/src/symbols.o $(test_objs)
