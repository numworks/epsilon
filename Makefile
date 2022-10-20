# Disable default Make rules
.SUFFIXES:

# Define the default recipe
default:

# Define a phony target with no rule. Targets that depend on it will always
# be remade
.PHONY: force_remake

include build/config.mak
include build/defaults.mak
include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak
include build/variants.mak
include build/helpers.mk

.PHONY: info
info:
	@echo "EPSILON_VERSION = $(EPSILON_VERSION)"
	@echo "EPSILON_APPS = $(EPSILON_APPS)"
	@echo "EPSILON_I18N = $(EPSILON_I18N)"
	@echo "PLATFORM" = $(PLATFORM)
	@echo "DEBUG" = $(DEBUG)
	@echo "NO_BOOTLOADER" = $(NO_BOOTLOADER)
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
	@echo "  make epsilon.onboarding.allow3rdparty.dfu"
	@echo "  make epsilon.onboarding.beta.allow3rdparty.dfu"
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
ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
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
include quiz/Makefile # Quiz needs to be included at the end

# Define main and shortcut targets
include build/targets.mak

all_src = $(apps_src) $(escher_src) $(ion_src) $(kandinsky_src) $(liba_src) $(libaxx_src) $(poincare_src) $(python_src) $(runner_src) $(ion_device_flasher_src) $(ion_device_bench_src) $(tests_src)

# Ensure kandinsky fonts are generated first
$(call object_for,$(all_src)): $(kandinsky_deps)

# kernel_obj are added separately since they require variants resolution
all_objs = $(call object_for,$(all_src)) $(kernel_obj)
.SECONDARY: $(all_objs)

# Load source-based dependencies
# Compilers can generate Makefiles that states the dependencies of a given
# objet to other source and headers. This serve no purpose for a clean build,
# but allows correct yet optimal incremental builds.
-include $(all_objs:.o=.d)

# Fill in the default recipe
default: $(firstword $(HANDY_TARGETS)).$(firstword $(HANDY_TARGETS_EXTENSIONS))

# Load standard build rules
include build/rules.mk

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR:/clean=)

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

