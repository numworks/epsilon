# Main compilation rules

# Load source-based dependencies.
# When compiling a source file, the compiler can emit makefile fragements
# adding prerequisites on headers included. This enables recompilation of a
# source file when the headers change.
-include $(shell find $(OUTPUT_DIRECTORY) -name '*.d' 2>/dev/null)

# Create the output directories tree
$(OUTPUT_DIRECTORY)/.:
	$(QUIET) mkdir -p $@
$(OUTPUT_DIRECTORY)%/.:
	$(QUIET) mkdir -p $@

# Rules for executable applications
$(OUTPUT_DIRECTORY)/%.$(EXECUTABLE_EXTENSION): $$(call libraries_for_flavored_goal,%) | $$(@D)/.
	$(QUIET) $(LD) $(SFLAGS) $^ $(LDFLAGS) -o $@

# Rules for modules as static libraries
$(OUTPUT_DIRECTORY)/%.a: $$(call objects_for_flavored_module,%) | $$(@D)/.
	$(QUIET) $(AR) $(ARFLAGS) $@ $^

# Rules for object files
$(OUTPUT_DIRECTORY)/%.o: %.c | $$(@D)/.
	$(QUIET) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIRECTORY)/%.o: %.cpp | $$(@D)/.
	$(QUIET) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

# Platform-specific rules
-include $(PATH_haussmann)/src/rules.$(PLATFORM).mak
