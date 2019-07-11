include scripts/config.mak

# Disable default Make rules
.SUFFIXES:

object_for = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))

default: $(BUILD_DIR)/epsilon.$(EXE)

# Define a standard rule helper
# If passed a last parameter value of with_local_version, we also define an
# extra rule that can build source files within the $(BUILD_DIR). This is useful
# for rules that can be applied for intermediate objects (for example, when
# going .png -> .cpp -> .o).

define rule_label
@ echo "$(shell printf "%-8s" $(strip $(1)))$(@:$(BUILD_DIR)/%=%)"
endef

define rule_for
$(addprefix $$(BUILD_DIR)/,$(strip $(2))): $(strip $(3)) | $$$$(@D)/.
	@ echo "$(shell printf "%-8s" $(strip $(1)))$$(@:$$(BUILD_DIR)/%=%)"
	$(Q) $(4)
ifeq ($(strip $(5)),with_local_version)
$(addprefix $$(BUILD_DIR)/,$(strip $(2))): $(addprefix $$(BUILD_DIR)/,$(strip $(3)))
	@ echo "$(shell printf "%-8s" $(strip $(1)))$$(@:$$(BUILD_DIR)/%=%)"
	$(Q) $(4)
endif
endef

.PHONY: info
info:
	@echo "EPSILON_VERSION = $(EPSILON_VERSION)"
	@echo "EPSILON_APPS = $(EPSILON_APPS)"
	@echo "EPSILON_I18N = $(EPSILON_I18N)"

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
include scripts/struct_layout/Makefile
include scripts/scenario/Makefile
include quiz/Makefile # Quiz needs to be included at the end

all_src = app_src escher_src ion_src kandinsky_src liba_src libaxx_src poincare_src python_src ion_device_dfu_relocated_src ion_device_dfu_xip epsilon_src runner_src flasher_src bench_src tests_src
all_objs = $(call object_for,$(all_src))
.SECONDARY: $(all_objs)

# Load source-based dependencies
# Compilers can generate Makefiles that states the dependencies of a given
# objet to other source and headers. This serve no purpose for a clean build,
# but allows correct yet optimal incremental builds.
-include $(all_objs:.o=.d)

executables = epsilon epsilon.on-boarding epsilon.on-boarding.update epsilon.on-boarding.beta test

#define platform generic targets
all_epsilon_common_src = $(ion_src) $(liba_src) $(kandinsky_src) $(epsilon_src) $(app_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(ion_device_dfu_relocated_src)
all_epsilon_default_src = $(all_epsilon_common_src) $(apps_launch_default_src) $(apps_prompt_none_src)

$(BUILD_DIR)/epsilon.$(EXE): $(call object_for,$(all_epsilon_default_src))
$(BUILD_DIR)/epsilon.on-boarding.$(EXE): $(call object_for,$(all_epsilon_common_src) $(apps_launch_on_boarding_src) $(apps_prompt_none_src))
$(BUILD_DIR)/epsilon.on-boarding.update.$(EXE): $(call object_for,$(all_epsilon_common_src) $(apps_launch_on_boarding_src) $(apps_prompt_update_src))
$(BUILD_DIR)/epsilon.on-boarding.beta.$(EXE): $(call object_for,$(all_epsilon_common_src) $(apps_launch_on_boarding_src) $(apps_prompt_beta_src))

$(BUILD_DIR)/test.$(EXE): $(BUILD_DIR)/quiz/src/tests_symbols.o $(call object_for,$(ion_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(ion_device_dfu_relocated_src) $(tests_src) $(runner_src) $(app_calculation_src) $(app_probability_src) $(app_regression_src) $(app_sequence_src) $(app_shared_src) $(app_statistics_src) $(app_solver_src))

# Load platform-specific targets
# We include them before the standard ones to give them precedence.
-include scripts/targets.$(PLATFORM).mak

$(foreach executable,$(executables),$(eval $(call rules_for_targets,$(executable),$(EXE))))

# Define standard compilation rules

$(eval $(call rule_for, \
  AS, %.o, %.s, \
  $$(CC) $$(SFLAGS) -c $$< -o $$@ \
))

$(eval $(call rule_for, \
  CC, %.o, %.c, \
  $$(CC) $$(SFLAGS) $$(CFLAGS) -c $$< -o $$@, \
  with_local_version \
))

$(eval $(call rule_for, \
  CXX, %.o, %.cpp, \
  $$(CXX) $$(SFLAGS) $$(CXXFLAGS) -c $$< -o $$@, \
  with_local_version \
))

$(eval $(call rule_for, \
  OCC, %.o, %.m, \
  $$(CC) $$(SFLAGS) $$(CFLAGS) -c $$< -o $$@ \
))

$(eval $(call rule_for, \
  LD, %.$$(EXE), , \
  $$(LD) $$^ $$(LDFLAGS) -o $$@ \
))

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

