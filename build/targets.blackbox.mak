# Compare

products += $(wildcard ion/src/blackbox/library_*.o)

ion/src/blackbox/library_%.o: SFLAGS += -D EPSILON_LIB_PREFIX=$(*F)
ion/src/blackbox/library_%.o: ion/src/blackbox/library.cpp
	@echo "CXX     $@"
	$(Q) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

libepsilon_objs = $(filter-out $(addprefix ion/src/blackbox/,boot.o events.o),$(objs))

libepsilon_%.o: LDFLAGS += -exported_symbols_list ion/src/blackbox/lib_export_list.txt
libepsilon_%.o: $(libepsilon_objs) $(app_objs) $(app_image_objs) ion/src/blackbox/library_%.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -r -s -o $@

compare: ion/src/blackbox/compare.o libepsilon_first.o libepsilon_second.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -L. -o $@

# Integration tests

.PHONY: tests/%.run
tests/%.run: tests/%.esc epsilon.$(EXE)
	@echo "RUN     $<"
	@./epsilon.$(EXE) --logAfter 0 < $< > /dev/null

.PHONY: tests/%.render
tests/%.render: tests/%.esc epsilon.$(EXE)
	@echo "RENDER  $<"
	@rm -rf tests/$(*F)
	@mkdir -p tests/$(*F)
	@rm -f event*.png
	@./epsilon.$(EXE) --logAfter 0 < $< > /dev/null
	@mv event*.png tests/$(*F)

scenarios = $(wildcard tests/*.esc)
.PHONY: integration_tests
integration_tests: $(scenarios:.esc=.run)

# Fuzzing
.PHONY: epsilon_fuzz
ifeq ($(TOOLCHAIN),afl)
epsilon_fuzz: epsilon.$(EXE)
	@echo "FUZZ    $<"
	@afl-fuzz -i tests -o afl ./epsilon.$(EXE)
else
epsilon_fuzz:
	@echo "Fuzzing requires TOOLCHAIN=afl"
endif
