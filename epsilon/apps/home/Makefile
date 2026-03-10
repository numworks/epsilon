SOURCES_apps += $(addprefix $(PATH_apps)/home/,$(addsuffix :-test, \
  app.cpp \
  app_cell.cpp \
  controller.cpp \
))

SOURCES_apps += $(call generated_sources_for,$(addprefix $(PATH_apps)/home/, \
  apps_layout.cpp:-test \
))

$(call i18n_without_universal,home/i18n/base)

$(OUTPUT_DIRECTORY)/$(PATH_apps)/home/apps_layout.h: $(addprefix $(PATH_apps)/home/apps_layout.,py csv) | $$(@D)/.
	$(call rule_label,APPSLYT)
	$(PYTHON) $(filter %.py,$^) --layouts $(filter %.csv,$^) --header $@ --implementation $(basename $@).cpp --apps $(EPSILON_APPS)

$(OUTPUT_DIRECTORY)/$(PATH_apps)/home/apps_layout.cpp: $(OUTPUT_DIRECTORY)/$(PATH_apps)/home/apps_layout.h

$(call all_objects_for,$(PATH_apps)/home/controller.cpp): $(OUTPUT_DIRECTORY)/$(PATH_apps)/home/apps_layout.h
