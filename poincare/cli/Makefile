# Copied from poincarejs, some parts could be factorized

# Assert this is being run from the repository's top level, as haussmann does
# not like having .. in paths.
ifeq ($(shell find . -maxdepth 1 -name '.git'),)
$(error For now, this makefile should be run from the root of epsilon. Use "make -f poincare/cli/Makefile ..." instead)
endif

PATH_haussmann := haussmann
APP_NAME := Poincare
APP_VERSION := 0.1.0
OUTPUT_ROOT := output
DEBUG ?= 1
PLATFORM ?= host
EMIT_TSD ?= 1
include $(PATH_haussmann)/Makefile

POINCARE_TREE_LOG = 1

ASSERTIONS ?= $(DEBUG)

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS)

ifeq ($(ASSERTIONS),0)
SFLAGS += -DNDEBUG
endif

# Modules

# FIXME: We shouldn't need Kandinsky font
KANDINSKY_font_variant := epsilon
POINCARE_variant = epsilon

# FIXME: Flags to build ion files
SFLAGS += \
  -Iion/include/ion/keyboard/epsilon \
  -DION_DISPLAY_WIDTH=1000 \
  -DION_DISPLAY_HEIGHT=1000 \
  -DION_DISPLAY_BORDER=0 \
  -DION_KEYBOARD_COLUMNS=6 \
  -DION_KEYBOARD_ROWS=9

$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,poincare,poincare)

# Module poincare_cli
$(call import_module,poincare_cli,.)
$(call create_module,poincare_cli,1, \
  $(addprefix poincare/cli/, \
    commands.cpp \
    main.cpp \
    timing.cpp \
  ) \
  $(addprefix ion/src/, \
   $(addprefix shared/, \
      exam_mode.cpp \
    ) \
    $(addprefix shared/dummy/, \
      authentication.cpp \
      exam_bytes.cpp \
      external_apps.cpp \
      init.cpp \
      led.cpp \
      reset.cpp \
    ) \
    $(addprefix simulator/shared/, \
      crc32.cpp \
      random.cpp \
      dummy/read_only_memory.cpp \
    ) \
  ) \
)

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_poincare_cli)): $(KANDINSKY_fonts_dependencies)

PRIVATE_SFLAGS_poincare_cli += \
  -Iion/include \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

LDFLAGS_poincare_cli += -ledit

# Rules

$(call create_goal,poincare_cli, \
  kandinsky \
  omg \
  poincare_cli \
)
