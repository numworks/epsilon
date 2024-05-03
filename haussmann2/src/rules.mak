# Main compilation rules

# Load source-based dependencies.
# When compiling a source file, the compiler can emit makefile fragments
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
	$(QUIET) echo "LD\t$@"
	$(QUIET) $(LD) $(SFLAGS) $^ $(LDFLAGS) -o $@

$(eval $(call document_extension,$(EXECUTABLE_EXTENSION)))

# Rules for modules as static libraries
$(OUTPUT_DIRECTORY)/%.a: $$(call objects_for_flavored_module,%) | $$(@D)/.
	$(QUIET) $(call check_locks,$(call name_for_flavored_target,$*))
	$(QUIET) echo "AR\t$@"
	$(QUIET) $(AR) $(ARFLAGS) $@ $^

# Rules for object files
$(OUTPUT_DIRECTORY)/%.o: %.c | $$(@D)/.
	$(QUIET) echo "CC\t$@"
	$(QUIET) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIRECTORY)/%.o: %.cpp | $$(@D)/.
	$(QUIET) echo "CXX\t$@"
	$(QUIET) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

# Lock files, ensure that modules versions match
%.lock:
	$(call lockfile_recipe,$*)

$(eval $(call document_extension,lock,Generate $(LOCKFILE_NAME) listing versions of modules used by the goal))

# Platform-specific rules
-include $(PATH_haussmann)/src/rules.$(PLATFORM).mak
