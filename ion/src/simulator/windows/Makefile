ion_src += $(addprefix ion/src/simulator/windows/, \
  images.cpp \
  language.cpp \
  resources.rc \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/callback.cpp \
  dummy/haptics.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

LDFLAGS += -lgdiplus

# The header is refered to as <ion/src/simulator/windows/resources.h> so make sure it's findable this way
SFLAGS += -I$(BUILD_DIR)

$(eval $(call rule_for, \
  WINDOWS_ASSETS, \
  ion/src/simulator/windows/resources.h, \
  $(assets_paths), \
  $$(PYTHON) ion/src/simulator/windows/assets.py --files $$^ --header-resource-rc $$@, \
  global \
))

$(eval $(call rule_for, \
  WINDOWS_ASSETS, \
  ion/src/simulator/windows/resource_mapping.h, \
  $(assets_paths), \
  $$(PYTHON) ion/src/simulator/windows/assets.py --files $$^ --header-resource-mapping $$@, \
  global \
))


$(BUILD_DIR)/ion/src/simulator/windows/images.o: $(BUILD_DIR)/ion/src/simulator/windows/resource_mapping.h
$(BUILD_DIR)/ion/src/simulator/windows/resources.o: $(BUILD_DIR)/ion/src/simulator/windows/resources.h

