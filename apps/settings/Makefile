SOURCES_apps += $(addprefix $(PATH_apps)/settings/, \
  app.cpp \
  main_controller.cpp \
  main_controller_prompt_beta.cpp:+beta \
  main_controller_prompt_none.cpp:-beta:-update \
  main_controller_prompt_update.cpp:+update \
$(addprefix sub_menu/, \
  about_controller.cpp \
  display_mode_controller.cpp \
  exam_mode_controller.cpp \
  generic_sub_controller.cpp \
  localization_controller.cpp \
  preferences_controller.cpp \
  press_to_test_controller.cpp \
  selectable_view_with_messages.cpp \
  test_mode_controller.cpp \
  press_to_test_success.png \
) \
  settings_icon.png \
)

$(call depends_on_image,$(PATH_apps)/settings/app.cpp,$(PATH_apps)/settings/settings_icon.png)
$(call depends_on_transparent_image,$(PATH_apps)/settings/sub_menu/press_to_test_controller.cpp,$(PATH_apps)/settings/sub_menu/press_to_test_success.png)

$(call i18n_with_universal,settings/i18n/base)
