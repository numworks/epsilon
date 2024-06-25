# Import haussmann

PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= macos

ALL_SPECIAL_SUBDIRECTORIES := safe_stack

include $(PATH_haussmann)/Makefile

# Further configuration

ASSERTIONS ?= $(DEBUG)
EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL) \

ifeq ($(PLATFORM_TYPE),device)
# TODO Sunset those flags as they make builds unsafe
DEVELOPMENT ?= 1
IN_FACTORY ?= 0
EMBED_EXTRA_DATA ?= 0

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
$(call import_module,quiz,quiz)

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

$(call create_goal,test, \
  apps.test \
  escher.test \
  ion.test \
  kandinsky.test \
  omg.test \
  poincare.test \
  python.test \
  quiz \
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

$(OUTPUT_DIRECTORY)/safe_stack/%: SFLAGS += -fstack-protector-strong

$(call create_goal,bootloader, \
  ion.bootloader \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  libsodium \
  omg.minimal.decompress \
,safe_stack, \
)

$(call create_goal,kernel, \
  ion.kernel \
  kandinsky.minimal \
  liba.armv7m \
  libaxx \
  omg.minimal \
,safe_stack, \
)

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

# Special target for the combined epsilon DFU
epsilon%dfu: $(OUTPUT_DIRECTORY)/epsilon%dfu
	@ :
epsilon%flash: $(OUTPUT_DIRECTORY)/epsilon%flash
	@ :

$(OUTPUT_DIRECTORY)/epsilon%dfu: $(addprefix $(OUTPUT_DIRECTORY)/,$(addprefix safe_stack/,bootloader.elf kernel.A.elf kernel.B.elf) userland.A%elf userland.B%elf)
	$(call rule_label,DFU)
	$(QUIET) $(PYTHON) $(PATH_haussmann)/data/device/elf2dfu.py -i $^ -o $@

$(call create_goal,flasher, \
  ion.flasher \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  omg.minimal \
)

ifeq ($(PLATFORM),n0120)
flasher%flash: DFULEAVE := 0x24030000
else
flasher%flash: DFULEAVE := 0x20030000
endif

endif
