# Main compilation rules

# Load source-based dependencies.
# When compiling a source file, the compiler can emit makefile fragments
# adding prerequisites on headers included. This enables recompilation of a
# source file when the headers change.
-include $(shell find $(OUTPUT_DIRECTORY) -name '*.d' 2>/dev/null)

# Create the output directories tree
$(OUTPUT_ROOT)/.:
	$(QUIET) mkdir -p $@
$(OUTPUT_ROOT)%/.:
	$(QUIET) mkdir -p $@

# Rules for executable applications
$(OUTPUT_DIRECTORY)/%.$(EXECUTABLE_EXTENSION): $$(call libraries_for_flavored_goal,%) | $$(@D)/.
	$(call rule_label,LD)
	$(QUIET) $(LD) $(PRIORITY_SFLAGS) $(SFLAGS) $^ $(call ldflags_for_flavored_goal,$*) -o $@

$(call document_extension,$(EXECUTABLE_EXTENSION))

# Rules for modules as static libraries
$(OUTPUT_DIRECTORY)/%.a: $$(call objects_for_flavored_module,%) | $$(@D)/.
	$(QUIET) $(call check_locks,$(call name_for_flavored_target,$*))
	$(call rule_label,AR)
	$(QUIET) $(AR) $(ARFLAGS) $@ $^

# Rules for object files
$(call rule_for_object, \
  CC, c, \
  $$(CC) $$(PRIORITY_SFLAGS) $$(SFLAGS) $$(CFLAGS) -c $$< -o $$@ \
)

$(call rule_for_object, \
  CXX, cpp, \
  $$(CXX) $$(PRIORITY_SFLAGS) $$(SFLAGS) $$(CXXFLAGS) -c $$< -o $$@ \
)

$(call rule_for_object, \
  AS, s, \
  $$(CC) $$(PRIORITY_SFLAGS) $$(SFLAGS) -c $$< -o $$@ \
)

# Lock files, ensure that modules versions match
%.lock:
	$(call lockfile_recipe,$*)

$(call document_extension,lock,Generate $(LOCKFILE_NAME) listing versions of modules used by the goal)

# Auxiliary binaries compiled for the host are built in their own directory.
include $(PATH_haussmann)/src/rules/tools.mak

# Platform-specific rules
-include $(PATH_haussmann)/src/rules/$(PLATFORM).mak
