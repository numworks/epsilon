# Compare

$(BUILD_DIR)/ion/src/blackbox/library_%.o: SFLAGS += -D EPSILON_LIB_PREFIX=$(*F)
$(BUILD_DIR)/ion/src/blackbox/library_%.o: ion/src/blackbox/library.cpp
	@echo "CXX     $@"
	$(Q) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

libepsilon_src = $(filter-out $(addprefix ion/src/blackbox/,boot.cpp events.cpp),$(src))

$(BUILD_DIR)/libepsilon_%.o: LDFLAGS += -exported_symbols_list ion/src/blackbox/lib_export_list.txt
$(BUILD_DIR)/libepsilon_%.o: $(call object_for,$(libepsilon_src)) $(call object_for,$(app_src)) $(BUILD_DIR)/ion/src/blackbox/library_%.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -r -s -o $@

$(BUILD_DIR)/compare: $(call object_for,ion/src/blackbox/compare.cpp)
	@echo "LD      $@"
	$(Q) $(LD) $^ libepsilon_first.o libepsilon_second.o $(LDFLAGS) -L. -o $@

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
epsilon_fuzz: $(BUILD_DIR)/epsilon.$(EXE)
	@echo "FUZZ    $<"
	@afl-fuzz -i tests -o afl $(BUILD_DIR)/epsilon.$(EXE)
else
epsilon_fuzz:
	@echo "Fuzzing requires TOOLCHAIN=afl"
endif

.PHONY: compare_fuzz
ifeq ($(TOOLCHAIN),afl)
compare_fuzz: $(BUILD_DIR)/compare
	@echo "FUZZ    $<"
	@afl-fuzz -t 3000 -i tests -o afl $(BUILD_DIR)/compare
else
compare_fuzz:
	@echo "Fuzzing requires TOOLCHAIN=afl"
endif
