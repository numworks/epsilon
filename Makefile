# Import haussmann

PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 1
PLATFORM ?= macos
include $(PATH_haussmann)/Makefile

# Further configuration

ASSERTIONS ?= $(DEBUG)
EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

ifeq ($(PLATFORM_TYPE),device)
SFLAGS += -DPLATFORM_DEVICE
endif

# Import modules

$(call import_module,liba,liba)
$(call import_module,libaxx,libaxx)
$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,ion,ion)
$(call import_module,poincare,poincare)
$(call import_module,escher,escher)
$(call import_module,python,python)
$(call import_module,apps,apps)

ifeq ($(PLATFORM_TYPE),simulator)
$(call import_module,sdl,ion/src/simulator/external)
endif

# Declare goals

ifeq ($(PLATFORM_TYPE),simulator)
$(call create_goal,epsilon, \
  apps \
  escher \
  ion \
  kandinsky \
  omg \
  poincare \
  python \
  sdl \
)
endif

ifeq ($(PLATFORM),web)
$(call create_zip,epsilon%zip,$(addprefix $(OUTPUT_DIRECTORY)/, \
  epsilon%js \
  ion/src/simulator/web/simulator.html \
  app/assets/background.jpg \
))

$(call create_zip,htmlpack%zip,$(addprefix $(OUTPUT_DIRECTORY)/, \
  epsilon%js \
  ion/src/simulator/web/calculator.html \
  ion/src/simulator/web/calculator.css \
  app/assets/background.jpg \
  app/assets/background-no-shadow.webp \
) \
  ion/src/simulator/assets/background-with-shadow.webp \
)

epsilon%html: $(OUTPUT_DIRECTORY)/epsilon%html
	@ :

$(OUTPUT_DIRECTORY)/epsilon%html: $(addprefix $(OUTPUT_DIRECTORY)/,epsilon%js ion/src/simulator/web/simulator.html app/assets/background.jpg) ion/src/simulator/assets/background-with-shadow.webp ion/src/simulator/web/inline.py
	$(call rule_label,INLINE)
	$(QUIET) $(filter %.py,$^) \
		--script $(filter %.js,$^) \
		--image $(filter %.webp,$^) \
		--image $(filter %.jpg,$^) \
		$(filter %.html,$^) >$@
endif

ifeq ($(PLATFORM_TYPE),device)
$(call create_goal,userland, \
  apps \
  escher \
  ion.userland \
  kandinsky \
  liba.aeabirt.armv7m.openbsd \
  libaxx \
  omg \
  poincare \
  python \
)
endif
