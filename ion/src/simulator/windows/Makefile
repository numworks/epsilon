ion_src += $(addprefix ion/src/simulator/windows/, \
  images.cpp \
  language.cpp \
  resources.rc \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/callback.cpp \
  dummy/haptics_enabled.cpp \
  haptics.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

# RC file dependencies
$(call object_for,ion/src/simulator/windows/resources.rc): WRFLAGS += -I $(BUILD_DIR)

# The header of images is refered to as <ion/src/simulator/windows/resources.h> so make sure it's findable this way
SFLAGS += -I$(BUILD_DIR)

LDFLAGS += -lgdiplus

$(eval $(call rule_for, \
  RESGEN, \
  ion/src/simulator/windows/resources_gen.rc ion/src/simulator/windows/images.h, \
  $(ion_simulator_assets_paths), \
  $$(PYTHON) ion/src/simulator/windows/resgen.py $(ion_simulator_assets) -o $$@, \
  global \
))

$(call object_for,ion/src/simulator/windows/images.cpp): $(BUILD_DIR)/ion/src/simulator/windows/images.h
$(call object_for,ion/src/simulator/windows/resources.rc): $(BUILD_DIR)/ion/src/simulator/windows/resources_gen.rc

