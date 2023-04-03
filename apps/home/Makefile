app_home_src = $(addprefix apps/home/,\
  app.cpp \
  app_cell.cpp \
  controller.cpp \
)

apps_src += $(app_home_src)

i18n_files += $(call i18n_without_universal_for,home/base)

# Apps layout file generation

# The header is refered to as <apps/home/apps_layout.h> so make sure it's
# findable this way
SFLAGS += -I$(BUILD_DIR)

apps_layout = apps/home/apps_layout.csv

$(eval $(call rule_for, \
  APPSLYT, \
  apps/home/apps_layout.h, \
  apps/home/apps_layout.py $(apps_layout), \
  $$(PYTHON) apps/home/apps_layout.py --layouts $(apps_layout) --header $$@ --implementation $$(subst .h,.cpp,$$@) --apps $$(EPSILON_APPS), \
  global \
))

$(BUILD_DIR)/apps/home/apps_layout.cpp: $(BUILD_DIR)/apps/home/apps_layout.h
apps_src += $(BUILD_DIR)/apps/home/apps_layout.cpp
