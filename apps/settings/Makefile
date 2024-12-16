SOURCES_apps += $(addprefix $(PATH_apps)/settings/,$(addsuffix :-test, \
  app.cpp \
  main_controller.cpp \
  main_controller_prompt_beta.cpp:+beta \
  main_controller_prompt_none.cpp:-beta:-update \
  main_controller_prompt_update.cpp:+update \
  sub_menu/about_controller.cpp \
  sub_menu/display_mode_controller.cpp \
  sub_menu/exam_mode_controller.cpp \
  sub_menu/generic_sub_controller.cpp \
  sub_menu/localization_controller.cpp \
  sub_menu/preferences_controller.cpp \
  sub_menu/press_to_test_controller.cpp \
  sub_menu/screen_timeout_controller.cpp \
  sub_menu/selectable_view_with_messages.cpp \
  sub_menu/test_mode_controller.cpp \
  sub_menu/press_to_test_success.png \
  settings_icon.png \
))

$(call depends_on_image,$(PATH_apps)/settings/app.cpp,$(PATH_apps)/settings/settings_icon.png)
$(call depends_on_transparent_image,$(PATH_apps)/settings/sub_menu/press_to_test_controller.cpp,$(PATH_apps)/settings/sub_menu/press_to_test_success.png)

$(call i18n_with_universal,settings/i18n/base)
