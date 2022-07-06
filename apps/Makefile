include apps/helpers.mk
include apps/shared/Makefile
include apps/home/Makefile
include apps/on_boarding/Makefile
include apps/hardware_test/Makefile
include apps/usb/Makefile

apps =
# All selected apps are included. Each Makefile below is responsible for setting
# the $apps variable (name of the app class) and the $app_headers
# (path to the apps header).
$(foreach i,${apps_list},${eval include apps/$(i)/Makefile})

app_equals = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
# We list all the apps that are missing
apps_missing = $(foreach i, ${apps_list}, $(if $(call app_equals, apps/$(i)/Makefile, $(wildcard apps/$(i)/Makefile)),, $(i) ) )

# If the two doesn't match, we got an error.
ifneq ($(strip $(apps_missing)),)
    miss_modules = 0

    # Check if the missing apps are one that are supposed to be submodules
    $(foreach i, $(SUBMODULES_APPS), $(if $(call app_equals, $(filter $(i), $(apps_missing)), $(i)), $(eval miss_modules=1)))

    ifeq ($(miss_modules), 1)
        PLS_IGNORE := $(shell >&2 printf "\nSome submodules apps seem to be missing. To download them, assuming you git clone'd the repo, do\n")
        PLS_IGNORE := $(shell >&2 printf "  git submodule init\n")
        PLS_IGNORE := $(shell >&2 printf "  git submodule update\n\n")
    endif
    $(error Missing apps: $(strip $(apps_missing)))
endif

apps_src += $(addprefix apps/,\
  alternate_empty_nested_menu_controller.cpp \
  apps_container.cpp \
  apps_container_launch_default.cpp:-onboarding \
  apps_container_launch_on_boarding.cpp:+onboarding \
  apps_container_prompt_beta.cpp:+beta \
  apps_container_prompt_none.cpp:-beta \
  apps_container_prompt_none.cpp:-update \
  apps_container_prompt_update.cpp:+update \
  apps_container_storage.cpp \
  apps_window.cpp \
  backlight_dimming_timer.cpp \
  battery_timer.cpp \
  battery_view.cpp \
  clock_timer.cpp \
  empty_battery_window.cpp \
  exam_pop_up_controller.cpp \
  exam_mode_configuration_official.cpp:+official \
  exam_mode_configuration_non_official.cpp:-official \
  global_preferences.cpp \
  i18n.py \
  lock_view.cpp \
  main.cpp \
  math_toolbox.cpp \
  math_variable_box_controller.cpp \
  math_variable_box_empty_controller.cpp \
  shift_alpha_lock_view.cpp \
  suspend_timer.cpp \
  timer_manager.cpp \
  title_bar_view.cpp \
  xnt_loop.cpp \
)

tests_src += $(addprefix apps/,\
  exam_mode_configuration_non_official.cpp \
)


snapshots_declaration = $(foreach i,$(apps),$(i)::Snapshot m_snapshot$(subst :,,$(i))Snapshot;)
apps_declaration = $(foreach i,$(apps),$(i) m_$(subst :,,$(i));)
snapshots_construction = $(foreach i,$(apps),,m_snapshot$(subst :,,$(i))Snapshot())
snapshots_list = $(foreach i,$(apps),,&m_snapshot$(subst :,,$(i))Snapshot)
snapshots_count = $(words $(apps))
snapshot_includes = $(foreach i,$(app_headers),-include $(i) )
epsilon_app_names = '$(foreach i,${apps_list},"$(i)", )'

$(call object_for,apps/apps_container_storage.cpp apps/apps_container.cpp apps/main.cpp): CXXFLAGS += $(snapshot_includes) -DAPPS_CONTAINER_APPS_DECLARATION="$(apps_declaration)" -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(snapshots_construction)" -DAPPS_CONTAINER_SNAPSHOT_LIST="$(snapshots_list)" -DAPPS_CONTAINER_SNAPSHOT_COUNT=$(snapshots_count) -DEPSILON_APPS_NAMES=$(epsilon_app_names) -DOMEGA_USERNAME="$(OMEGA_USERNAME)"

# I18n file generation

country_preferences = apps/country_preferences.csv
language_preferences = apps/language_preferences.csv

# The header is referred to as <apps/i18n.h> so make sure it's findable this way
SFLAGS += -I$(BUILD_DIR)

i18n_files += $(addprefix apps/language_,$(addsuffix .universal.i18n, $(EPSILON_I18N)))

i18n_files += $(call i18n_with_universal_for,shared)
i18n_files += $(call i18n_with_universal_for,toolbox)
i18n_files += $(call i18n_without_universal_for,variables)

$(eval $(call rule_for, \
  I18N, \
  apps/i18n.cpp, \
  $(i18n_files), \
  $$(PYTHON) apps/i18n.py --codepoints $(code_points) --countrypreferences $(country_preferences) --languagepreferences $(language_preferences) --header $$(subst .cpp,.h,$$@) --implementation $$@ --locales $$(EPSILON_I18N) --countries $$(EPSILON_COUNTRIES) --files $$^, \
  global \
))



$(BUILD_DIR)/apps/i18n.h: $(BUILD_DIR)/apps/i18n.cpp

# Handle PNG files

$(eval $(call depends_on_image,apps/title_bar_view.cpp,apps/exam_icon.png))

$(call object_for,$(apps_src) $(tests_src)): $(BUILD_DIR)/apps/i18n.h
$(call object_for,$(apps_src) $(tests_src)): $(BUILD_DIR)/apps/home/apps_layout.h
$(call object_for,$(apps_src) $(tests_src)): $(BUILD_DIR)/python/port/genhdr/qstrdefs.generated.h

$(call object_for,$(apps_src)): $(BUILD_DIR)/apps/home/apps_layout.h

apps_tests_src = $(app_calculation_test_src) $(app_code_test_src) $(app_graph_test_src) $(app_probability_test_src) $(app_regression_test_src) $(app_sequence_test_src) $(app_shared_test_src) $(app_statistics_test_src) $(app_settings_test_src) $(app_solver_test_src)

apps_tests_src += $(addprefix apps/,\
  alternate_empty_nested_menu_controller.cpp \
  global_preferences.cpp \
  dummy_timer_manager.cpp \
)

ifeq ($(THEME_REPO),local)
$(foreach img,$(image_list), $(eval $(call rule_for, \
  ICON, \
  $(img), \
  $(addprefix themes/themes/local/, $(addsuffix .json, $(THEME_NAME))), \
  $$(PYTHON) themes/themes_manager.py -i $(THEME_REPO) $(THEME_NAME) $$@ $(BUILD_DIR)/, \
  global \
)))
else
$(foreach img,$(image_list), $(eval $(call rule_for, \
  ICON, \
  $(img), \
  $(addsuffix /escher/palette.h, $(BUILD_DIR)), \
  $$(PYTHON) themes/themes_manager.py -i $(THEME_REPO) $(THEME_NAME) $$@ $(BUILD_DIR)/, \
  global \
)))
endif
