apps += Settings::App
app_headers += apps/settings/app.h

app_settings_src = $(addprefix apps/settings/,\
  app.cpp \
  cell_with_separator.cpp \
  main_controller.cpp \
  main_controller_prompt_beta.cpp:+beta \
  main_controller_prompt_none.cpp:-beta \
  main_controller_prompt_none.cpp:-update \
  main_controller_prompt_update.cpp:+update \
  sub_menu/about_controller.cpp \
  sub_menu/accessibility_controller.cpp \
  sub_menu/code_options_controller.cpp \
  sub_menu/about_controller_official.cpp:+official \
  sub_menu/about_controller_non_official.cpp:-official \
  sub_menu/exam_mode_controller_official.cpp:+official \
  sub_menu/exam_mode_controller_non_official.cpp:-official \
  sub_menu/datetime_controller.cpp \
  sub_menu/display_mode_controller.cpp \
  sub_menu/exam_mode_controller.cpp \
  sub_menu/generic_sub_controller.cpp \
  sub_menu/localization_controller.cpp \
  sub_menu/preferences_controller.cpp \
	sub_menu/contributors_controller.cpp \
	sub_menu/math_options_controller.cpp \
  sub_menu/selectable_view_with_messages.cpp \
  sub_menu/usb_protection_controller.cpp \
  sub_menu/brightness_controller.cpp\
)

SFLAGS += -DOMEGA_STATE="$(OMEGA_STATE)"

app_settings_src += $(app_settings_test_src)
apps_src += $(app_settings_src)

i18n_files += $(call i18n_with_universal_for,settings/base)

$(eval $(call depends_on_image,apps/settings/app.cpp,apps/settings/settings_icon.png))
