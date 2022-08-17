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
$(foreach i,${EPSILON_APPS},$(eval include apps/$(i)/Makefile))

apps_src += $(addprefix apps/,\
  alternate_empty_nested_menu_controller.cpp \
  apps_container.cpp \
  apps_container_helper.cpp \
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
  empty_battery_window.cpp \
  exam_pop_up_controller.cpp \
  exam_mode_configuration.cpp \
  global_preferences.cpp \
  i18n.py \
  lock_view.cpp \
  main.cpp \
  math_toolbox.cpp \
  math_variable_box_controller.cpp \
  math_variable_box_empty_controller.cpp \
  shift_alpha_lock_view.cpp \
  suspend_timer.cpp \
  title_bar_view.cpp \
)

tests_src += $(addprefix apps/,\
  apps_container_helper_tests.cpp \
  exam_mode_configuration.cpp \
)


snapshots_declaration = $(foreach i,$(apps),$(i)::Snapshot m_snapshot$(subst :,,$(i))Snapshot;)
apps_declaration = $(foreach i,$(apps),$(i) m_$(subst :,,$(i));)
snapshots_construction = $(foreach i,$(apps),,m_snapshot$(subst :,,$(i))Snapshot())
snapshots_list = $(foreach i,$(apps),,&m_snapshot$(subst :,,$(i))Snapshot)
snapshots_count = $(words $(apps))
snapshot_includes = $(foreach i,$(app_headers),-include $(i) )
epsilon_app_names = '$(foreach i,${EPSILON_APPS},"$(i)", )'

$(call object_for,apps/apps_container_storage.cpp apps/apps_container.cpp apps/main.cpp): CXXFLAGS += $(snapshot_includes) -DAPPS_CONTAINER_APPS_DECLARATION="$(apps_declaration)" -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(snapshots_construction)" -DAPPS_CONTAINER_SNAPSHOT_LIST="$(snapshots_list)" -DAPPS_CONTAINER_SNAPSHOT_COUNT=$(snapshots_count) -DEPSILON_APPS_NAMES=$(epsilon_app_names)

# I18n file generation

country_preferences = apps/country_preferences.csv
language_preferences = apps/language_preferences.csv

# The header is refered to as <apps/i18n.h> so make sure it's findable this way
SFLAGS += -I$(BUILD_DIR)

i18n_files += $(addprefix apps/language_,$(addsuffix .universal.i18n, $(EPSILON_I18N)))

i18n_files += $(call i18n_with_universal_for,shared)
i18n_files += $(call i18n_with_universal_for,toolbox)
i18n_files += $(call i18n_without_universal_for,variables)

$(eval $(call rule_for, \
  I18N, \
  apps/i18n.cpp, \
  $(i18n_files) $(country_preferences) $(language_preferences), \
  $$(PYTHON) apps/i18n.py --codepoints $(code_points) --countrypreferences $(country_preferences) --languagepreferences $(language_preferences) --header $$(subst .cpp,.h,$$@) --implementation $$@ --locales $$(EPSILON_I18N) --countries $$(EPSILON_COUNTRIES) --files $(i18n_files) $(if $(filter $(I18N_COMPRESS),1),--compress,), \
  global \
))

$(BUILD_DIR)/apps/i18n.h: $(BUILD_DIR)/apps/i18n.cpp

# Handle PNG files

$(eval $(call depends_on_image,apps/title_bar_view.cpp,apps/exam_icon.png))

$(call object_for,$(apps_src) $(tests_src)): $(BUILD_DIR)/apps/i18n.h
$(call object_for,$(apps_src) $(tests_src)): $(BUILD_DIR)/python/port/genhdr/qstrdefs.generated.h

$(call object_for,$(apps_src)): $(BUILD_DIR)/apps/home/apps_layout.h

apps_tests_src = $(app_calculation_test_src) $(app_code_test_src) $(app_graph_test_src) $(app_inference_test_src) $(app_regression_test_src) $(app_sequence_test_src) $(app_shared_test_src) $(app_statistics_test_src) $(app_settings_test_src) $(app_solver_test_src) $(app_finance_test_src)

apps_tests_src += $(addprefix apps/,\
  alternate_empty_nested_menu_controller.cpp \
  global_preferences.cpp \
)
