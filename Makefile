include build/config.mak

# Disable default Make rules
.SUFFIXES:

object_for = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))

# Define the default recipe

default:

# Define a standard rule helper
# If passed a last parameter value of with_local_version, we also define an
# extra rule that can build source files within the $(BUILD_DIR). This is useful
# for rules that can be applied for intermediate objects (for example, when
# going .png -> .cpp -> .o).

define rule_label
@ echo "$(shell printf "%-8s" $(strip $(1)))$(@:$(BUILD_DIR)/%=%)"
endef

define rule_for
ifeq ($(strip $(5)),with_local_version)
$(addprefix $$(BUILD_DIR)/,$(strip $(2))): $(addprefix $$(BUILD_DIR)/,$(strip $(3))) | $(if $(findstring official,${MAKECMDGOALS}),official_authorization)
	@ echo "$(shell printf "%-8s" $(strip $(1)))$$(@:$$(BUILD_DIR)/%=%)"
	$(Q) $(4)
endif
$(addprefix $$(BUILD_DIR)/,$(strip $(2))): $(strip $(3)) | $$$$(@D)/. $(if $(findstring official,${MAKECMDGOALS}),official_authorization)
	@ echo "$(shell printf "%-8s" $(strip $(1)))$$(@:$$(BUILD_DIR)/%=%)"
	$(Q) $(4)
endef

.PHONY: info
info:
	@echo "EPSILON_VERSION = $(EPSILON_VERSION)"
	@echo "EPSILON_APPS = $(EPSILON_APPS)"
	@echo "EPSILON_I18N = $(EPSILON_I18N)"
	@echo "PLATFORM" = $(PLATFORM)
	@echo "DEBUG" = $(DEBUG)
	@echo "EPSILON_GETOPT" = $(EPSILON_GETOPT)
	@echo "ESCHER_LOG_EVENTS_BINARY" = $(ESCHER_LOG_EVENTS_BINARY)
	@echo "QUIZ_USE_CONSOLE" = $(QUIZ_USE_CONSOLE)
	@echo "ION_STORAGE_LOG" = $(ION_STORAGE_LOG)
	@echo "POINCARE_TREE_LOG" = $(POINCARE_TREE_LOG)
	@echo "POINCARE_TESTS_PRINT_EXPRESSIONS" = $(POINCARE_TESTS_PRINT_EXPRESSIONS)

.PHONY: help
help:
	@echo "Device targets"
	@echo "  make epsilon_flash"
	@echo "  make epsilon.dfu"
	@echo "  make epsilon.onboarding.dfu"
	@echo "  make epsilon.onboarding.update.dfu"
	@echo "  make epsilon.onboarding.beta.dfu"
	@echo "  make flasher.light.bin"
	@echo "  make flasher.verbose.dfu"
	@echo "  make bench.ram.bin"
	@echo "  make bench.flash.bin"
	@echo "  make binpack"
	@echo ""
	@echo "Simulator targets"
	@echo "  make PLATFORM=simulator"
	@echo "  make PLATFORM=simulator TARGET=android"
	@echo "  make PLATFORM=simulator TARGET=ios"
	@echo "  make PLATFORM=simulator TARGET=macos"
	@echo "  make PLATFORM=simulator TARGET=web"
	@echo "  make PLATFORM=simulator TARGET=windows"

# Since we're building out-of-tree, we need to make sure the output directories
# are created, otherwise the receipes will fail (e.g. gcc will fail to create
# "output/foo/bar.o" because the directory "output/foo" doesn't exist).
# We need to mark those directories as precious, otherwise Make will try to get
# rid of them upon completion (and fail, since those folders won't be empty).
.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
$(BUILD_DIR)/.:
	$(Q) mkdir -p $(dir $@)
$(BUILD_DIR)%/.:
	$(Q) mkdir -p $(dir $@)

# To make objects dependent on their directory, we need a second expansion
.SECONDEXPANSION:

# Each sub-Makefile can either add sources to $(%_src) variables or define a
# new executable target. The $(%_src) variables list the sources that can be
# built and linked to executables being generated.

ifeq ($(USE_LIBA),0)
include liba/Makefile.bridge
else
SFLAGS += -ffreestanding -nostdinc -nostdlib
include liba/Makefile
include libaxx/Makefile
endif
include ion/Makefile
include kandinsky/Makefile
include poincare/Makefile
include python/Makefile
include escher/Makefile
# Executable Makefiles
include apps/Makefile
include build/struct_layout/Makefile
include build/scenario/Makefile
include quiz/Makefile # Quiz needs to be included at the end

all_src = $(apps_all_src) $(escher_src) $(ion_all_src) $(kandinsky_src) $(liba_src) $(libaxx_src) $(poincare_src) $(python_src) $(runner_src) $(ion_target_device_flasher_light_src) $(ion_target_device_flasher_verbose_src) $(ion_target_device_bench_src) $(tests_src)
all_objs = $(call object_for,$(all_src))
.SECONDARY: $(all_objs)

# Load source-based dependencies
# Compilers can generate Makefiles that states the dependencies of a given
# objet to other source and headers. This serve no purpose for a clean build,
# but allows correct yet optimal incremental builds.
-include $(all_objs:.o=.d)

# Define main and shortcut targets
include build/targets.mak

# Fill in the default recipe
DEFAULT ?= $(BUILD_DIR)/epsilon.$(EXE)
default: $(DEFAULT)

# Load standard build rules
include build/rules.mk

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR)

.PHONY: cowsay_%
cowsay_%:
	@echo " -------"
	@echo "| $(*F) |"
	@echo " -------"
	@echo "        \\   ^__^"
	@echo "         \\  (oo)\\_______"
	@echo "            (__)\\       )\\/\\"
	@echo "                ||----w |"
	@echo "                ||     ||"

.PHONY: clena
clena: cowsay_CLENA clean

