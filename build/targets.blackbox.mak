# Compare

HANDY_TARGETS += libepsilon_first libepsilon_second
HANDY_TARGETS_EXTENSIONS += o

$(eval $(call rules_for_targets,compare,))

# TODO: find a way to use rules define by rule_for instead of redeclaring them (we can't use them now because of the different basenames of the object and the source)

$(BUILD_DIR)/ion/src/blackbox/library_%.o: SFLAGS += -D EPSILON_LIB_PREFIX=$(*F)
$(BUILD_DIR)/ion/src/blackbox/library_%.o: ion/src/blackbox/library.cpp $(@D)/.
	@echo "CXX     $@"
	$(Q) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

libepsilon_src = $(filter-out $(addprefix ion/src/blackbox/,boot.cpp events.cpp),$(all_epsilon_default_src))

$(BUILD_DIR)/libepsilon_%.o: LDFLAGS += -exported_symbols_list ion/src/blackbox/lib_export_list.txt
$(BUILD_DIR)/libepsilon_%.o: $(call object_for,$(libepsilon_src)) $(BUILD_DIR)/ion/src/blackbox/library_%.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -r -s -o $@

$(BUILD_DIR)/compare: $(call object_for,ion/src/blackbox/compare.cpp)
	@echo "LD      $@"
	$(Q) $(LD) $^ $(BUILD_DIR)/libepsilon_first.o $(BUILD_DIR)/libepsilon_second.o $(LDFLAGS) -L. -o $@

# Integration tests

.PHONY: tests/%.run
tests/%.run: tests/%.esc epsilon.$(EXE)
	@echo "RUN     $<"
	@./$(BUILD_DIR)/epsilon.$(EXE) --logAfter 0 < $< > /dev/null

.PHONY: tests/%.render
tests/%.render: tests/%.esc epsilon.$(EXE)
	@echo "RENDER  $<"
	@rm -rf tests/$(*F)
	@mkdir -p tests/$(*F)
	@rm -f event*.png
	@./$(BUILD_DIR)/epsilon.$(EXE) --logAfter 0 < $< > /dev/null
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
