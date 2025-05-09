# Main compilation rules

# Load source-based dependencies.
# When compiling a source file, the compiler can emit makefile fragments
# adding prerequisites on headers included. This enables recompilation of a
# source file when the headers change.
-include $(shell find $(OUTPUT_DIRECTORY) -name '*.d' 2>/dev/null)

# Create the output directories tree
$(OUTPUT_ROOT)/.:
	mkdir -p $@
$(OUTPUT_ROOT)%/.:
	mkdir -p $@

# Rules for executable applications
ifeq ($(OS),Windows_NT)
# Work around command-line length limit
# On Msys2 the max command line is 32 000 characters. Our standard LD command
# can be longer than that because we have quite a lot of object files. To work
# around this issue, we write the object list in a "target.objs" file, and tell
# the linker to read its arguments from this file.
$(OUTPUT_DIRECTORY)/%.$(EXECUTABLE_EXTENSION): $$(call libraries_for_flavored_goal,%) $$(call lddeps_for_flavored_goal,%) | $$(@D)/.
	$(call rule_label,LD)
	$(call LD_WRAPPER_$(GOAL), \
		echo "$(foreach l,$(filter %.a,$^),$(call objects_for_flavored_module,$(patsubst $(OUTPUT_DIRECTORY)/%.a,%,$l)))" > $@.objs && \
		$(LD) $(PRIORITY_SFLAGS) $(SFLAGS) \
		@$@.objs \
		$(call ldflags_for_flavored_goal,$*) \
		-o $@ && rm $@.objs)
else
$(OUTPUT_DIRECTORY)/%.$(EXECUTABLE_EXTENSION): $$(call libraries_for_flavored_goal,%) $$(call lddeps_for_flavored_goal,%) | $$(@D)/.
	$(call rule_label,LD)
	$(call LD_WRAPPER_$(GOAL),$(LD) \
		$(PRIORITY_SFLAGS) $(SFLAGS) \
		$(foreach l,$(filter %.a,$^),$(call objects_for_flavored_module,$(patsubst $(OUTPUT_DIRECTORY)/%.a,%,$l))) \
		$(call ldflags_for_flavored_goal,$*) \
		-o $@)
endif

$(call document_extension,$(EXECUTABLE_EXTENSION))

# Rules for modules as static libraries
# Because of some quirks with how make selects rules, AR may be called event if
# some .o cannot be built. It will end up throwing an error that stops make but
# the .a might still be created. As such, we need to remove it manually and
# propagate the error.
$(OUTPUT_DIRECTORY)/%.a: $$(call objects_for_flavored_module,%) | $$(@D)/.
	$(call rule_label,AR)
	$(AR) $(ARFLAGS) $@ $^ || (rm -f $@; false)

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

$(call rule_for_object, \
  WINDRES, rc, \
  $$(WINDRES) $$(WRFLAGS) $$< -O coff -o $$@ \
)

# Auxiliary binaries compiled for the host are built in their own directory.
include $(PATH_haussmann)/src/rules/tools.mak

# Platform-specific rules
-include $(PATH_haussmann)/src/rules/$(PLATFORM).mak
