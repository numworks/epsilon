$(call create_module,apps,1, \
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
  global_preferences.cpp \
  init.cpp \
  lock_view.cpp \
  main.cpp \
  shift_alpha_lock_view.cpp \
  suspend_timer.cpp \
  title_bar_view.cpp \
  apps_container_helper_tests.cpp:+test \
  init_tests.cpp:+test \
)

$(call depends_on_image,$(PATH_apps)/title_bar_view.cpp,$(PATH_apps)/exam_icon.png)

# Include the specific apps
# FIXME Use flavors instead of EPSILON_APPS?
EPSILON_APPS ?= calculation graph code statistics distributions inference solver sequence regression elements finance settings

_apps_classes := $(foreach a,$(EPSILON_APPS),$(call capitalize,$a)::App)
_apps_snapshots_declaration := $(foreach a,$(_apps_classes),$a::Snapshot m_snapshot$(subst :,,$a)Snapshot;)
_apps_declaration := $(foreach a,$(_apps_classes),$a m_$(subst :,,$a);)
_apps_snapshots_construction := $(foreach a,$(_apps_classes),,m_snapshot$(subst :,,$a)Snapshot())
_apps_snapshots_list := $(foreach a,$(_apps_classes),,&m_snapshot$(subst :,,$a)Snapshot)
_apps_count := $(words $(EPSILON_APPS))
_apps_includes_flags := $(patsubst %,-include $(PATH_apps)/%/app.h,$(EPSILON_APPS))
_apps_names := $(foreach a,$(EPSILON_APPS),"$a",)

_apps_snapshot_cxxflags := \
  $(_apps_includes_flags) \
  -DAPPS_CONTAINER_APPS_DECLARATION="$(_apps_declaration)" \
  -DAPPS_CONTAINER_SNAPSHOT_DECLARATION="$(_apps_snapshots_declaration)" \
  -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(_apps_snapshots_construction)" \
  -DAPPS_CONTAINER_SNAPSHOT_LIST="$(_apps_snapshots_list)" \
  -DAPPS_CONTAINER_SNAPSHOT_COUNT="$(_apps_snapshots_count)" \
  -DEPSILON_APPS_NAMES='$(_apps_names)'

$(call objects_foreach_arch,$(addprefix $(PATH_apps)/,apps_container.cpp apps_container_storage.cpp init.cpp main.cpp)): CXXFLAGS += $(_apps_snapshot_cxxflags)

include $(patsubst %,$(PATH_apps)/%/Makefile,$(EPSILON_APPS))

# Generate I18n files
include $(PATH_apps)/i18n.mak

$(foreach l,$(EPSILON_I18N),$(call i18n_for_locale,language_$l,universal))
$(call i18n_with_universal,shared)
$(call i18n_with_universal,toolbox)
$(call i18n_without_universal,variables)

