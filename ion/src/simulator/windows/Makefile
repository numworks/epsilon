ION_SIMULATOR_FILES = 1

ion_src += $(addprefix ion/src/simulator/windows/, \
  platform_files.cpp \
  platform_images.cpp \
  platform_language.cpp \
  resources.rc \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/haptics_enabled.cpp \
  dummy/keyboard_callback.cpp \
  dummy/window_callback.cpp \
  circuit_breaker.cpp \
  clipboard_helper.cpp \
  haptics.cpp \
  journal.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

# RC file dependencies
$(call object_for,ion/src/simulator/windows/resources.rc): WRFLAGS += -I $(BUILD_DIR)
$(call object_for,ion/src/simulator/windows/resources.rc): $(addprefix $(BUILD_DIR)/,logo.ico)

$(addprefix $(BUILD_DIR)/,logo.ico): ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" -resize 256x256 $< $@

# Linker flags
LDFLAGS += -lgdiplus -lcomdlg32

$(eval $(call rule_for, \
  RESGEN, \
  ion/src/simulator/windows/resources_gen.rc ion/src/simulator/windows/images.h, \
  $(ion_simulator_assets_paths), \
  $$(PYTHON) ion/src/simulator/windows/resgen.py $(foreach asset_path,$(ion_simulator_assets_paths),$(notdir $(asset_path))) -o $$@, \
  global \
))

$(call object_for,ion/src/simulator/windows/platform_images.cpp): $(BUILD_DIR)/ion/src/simulator/windows/images.h
$(call object_for,ion/src/simulator/windows/resources.rc): $(BUILD_DIR)/ion/src/simulator/windows/resources_gen.rc
