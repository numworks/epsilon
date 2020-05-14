ion_src += $(addprefix ion/src/simulator/windows/, \
  images.cpp \
  language.cpp \
  resources.rc \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/callback.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

LDFLAGS += -lgdiplus
