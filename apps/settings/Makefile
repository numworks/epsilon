apps += Settings::App
app_headers += apps/settings/app.h

app_settings_src = $(addprefix apps/settings/,\
  app.cpp \
  cell_with_separator.cpp \
  main_controller.cpp \
  sub_menu/about_controller.cpp \
  sub_menu/display_mode_controller.cpp \
  sub_menu/exam_mode_controller.cpp \
  sub_menu/generic_sub_controller.cpp \
  sub_menu/language_controller.cpp \
  sub_menu/preferences_controller.cpp \
  sub_menu/selectable_view_with_messages.cpp \
)

app_settings_src += $(app_settings_test_src)
app_src += $(app_settings_src)

apps_prompt_none_src += apps/settings/main_controller_prompt_none.cpp
apps_prompt_beta_src += apps/settings/main_controller_prompt_beta.cpp
apps_prompt_update_src += apps/settings/main_controller_prompt_update.cpp

apps_settings_official += $(addprefix apps/settings/,\
  sub_menu/about_controller_official.cpp \
  sub_menu/exam_mode_controller_official.cpp \
)

apps_settings_non_official += $(addprefix apps/settings/,\
  sub_menu/about_controller_non_official.cpp \
  sub_menu/exam_mode_controller_non_official.cpp \
)

apps_official += $(apps_settings_official)
apps_non_official += $(apps_settings_non_official)

i18n_files += $(addprefix apps/settings/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

$(eval $(call depends_on_image,apps/settings/app.cpp,apps/settings/settings_icon.png))
