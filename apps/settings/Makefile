apps += Settings::App
app_headers += apps/settings/app.h

app_settings_src = $(addprefix apps/settings/,\
  app.cpp \
  main_controller.cpp \
  main_controller_prompt_beta.cpp:+beta \
  main_controller_prompt_none.cpp:-beta \
  main_controller_prompt_none.cpp:-update \
  main_controller_prompt_update.cpp:+update \
  sub_menu/about_controller.cpp \
  sub_menu/display_mode_controller.cpp \
  sub_menu/exam_mode_controller.cpp \
  sub_menu/generic_sub_controller.cpp \
  sub_menu/localization_controller.cpp \
  sub_menu/preferences_controller.cpp \
  sub_menu/press_to_test_controller.cpp \
  sub_menu/selectable_view_with_messages.cpp \
  sub_menu/test_mode_controller.cpp \
)

app_settings_src += $(app_settings_test_src)
apps_src += $(app_settings_src)

i18n_files += $(call i18n_with_universal_for,settings/base)

$(eval $(call depends_on_image,apps/settings/app.cpp,apps/settings/settings_icon.png))
$(eval $(call depends_on_transparent_image,apps/settings/sub_menu/press_to_test_controller.cpp,apps/settings/sub_menu/press_to_test_success.png))
