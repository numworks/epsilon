# Disable default Make rules
.SUFFIXES:

# Define the default recipe
default:

include build/config.mak
include build/pimp.mak
include build/defaults.mak
include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak
include build/variants.mak
include build/helpers.mk

ifeq (${MODEL}, n0100)
  ifeq ($(filter reader,$(apps_list)),)
  	$(warning reader app included, removing it on n0100. )
  	EPSILON_APPS := $(filter-out reader,$(EPSILON_APPS))
  endif
  ifneq ($(words $(EPSILON_I18N)), 1)
  	$(warning Only use 1 language on n0100, defaulting to en. )
  	EPSILON_I18N := en
  endif
  ifeq ($(INCLUDE_ULAB), 1)
    $(warning Removing uLab on n0100. )
    INCLUDE_ULAB := 0
  endif
endif

ifeq ($(filter reader,$(apps_list)),)
  HAS_READER := 1
endif

ifeq (${MODEL}, n0110)
  apps_list = ${EPSILON_APPS}
else
  ifeq (${MODEL},bootloader)
    apps_list = ${EPSILON_APPS}
  else
    apps_list = $(foreach i, ${EPSILON_APPS}, $(if $(filter external, $(i)),,$(i)))
  endif
endif

ifdef FORCE_EXTERNAL
  apps_list = ${EPSILON_APPS}
endif

ifeq ($(INCLUDE_ULAB), 1)
  SFLAGS += -DINCLUDE_ULAB
endif

ifdef HOME_DISPLAY_EXTERNALS
  ifneq ($(filter external,$(apps_list)),)
    SFLAGS += -DHOME_DISPLAY_EXTERNALS
  else
    $(warning HOME_DISPLAY_EXTERNALS is set but external isn't included, ignoring flag.)
  endif
endif

.PHONY: info
info:
	@echo "EPSILON_VERSION = $(EPSILON_VERSION)"
	@echo "EPSILON_APPS = $(EPSILON_APPS)"
	@echo "EPSILON_I18N = $(EPSILON_I18N)"
	@echo "THEME_NAME = $(THEME_NAME)"
	@echo "THEME_REPO = $(THEME_REPO)"
	@echo "BUILD_DIR = $(BUILD_DIR)"
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
	@echo "  make PLATFORM=simulator TARGET=3ds"

.PHONY: doc
doc:
	@echo "DOXYGEN"
	@mkdir -p output/doc/
	$(Q) doxygen build/doc/Doxyfile

.PHONY: print-%
print-%:
	@echo $* = $($*)
	@echo $*\'s origin is $(origin $*)

# Since we're building out-of-tree, we need to make sure the output directories
# are created, otherwise the recipes will fail (e.g. gcc will fail to create
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
include bootloader/Makefile
include apps/Makefile
include build/struct_layout/Makefile
include build/scenario/Makefile
include quiz/Makefile # Quiz needs to be included at the end

all_src = $(apps_src) $(escher_src) $(ion_src) $(kandinsky_src) $(liba_src) $(libaxx_src) $(poincare_src) $(python_src) $(runner_src) $(ion_device_flasher_src) $(ion_device_bench_src) $(tests_src)
# Make palette.h a dep for every source-file.
# This ensures that the theming engine works correctly.
$(call object_for,$(all_src)): $(BUILD_DIR)/escher/palette.h $(BUILD_DIR)/apps/i18n.h

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
default: $(firstword $(HANDY_TARGETS)).$(firstword $(HANDY_TARGETS_EXTENSIONS))

# Load standard build rules
include build/rules.mk

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR)

.PHONY: cleanall
cleanall:
	@echo "CLEANALL"
	$(Q) rm -rf output

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

.PHONY: compile
compile: output/$(BUILD_TYPE)/simulator/$(HOST)/epsilon.$(EXE)

.PHONY: cleanandcompile
cleanandcompile:
	${MAKE} cleanall
	${MAKE} compile

.PHONY: start
start:
	@echo "INFO Starting output/$(BUILD_TYPE)/simulator/$(HOST)/epsilon.$(EXE)"
	@$(Q) output/$(BUILD_TYPE)/simulator/$(HOST)/epsilon.$(EXE) -v

.PHONY: clean_run
clean_run: cleanandcompile
	${MAKE} start

.PHONY: run
run: compile
	${MAKE} start

.PHONY: translations
translations:
	@echo "TRANSLATIONS"
	$(Q) ${PYTHON} build/utilities/translate.py

.PHONY: translations_clean
translations_clean:
	@echo "TRANSLATIONS CLEAN"
	$(Q) ${PYTHON} build/utilities/translations_clean.py
