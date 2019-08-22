apps += Settings::App
app_headers += apps/settings/app.h

app_src += $(addprefix apps/settings/,\
  app.cpp \
  main_controller.cpp \
  settings_message_tree.cpp \
  sub_menu/about_controller.cpp \
  sub_menu/display_mode_controller.cpp \
  sub_menu/exam_mode_controller.cpp \
  sub_menu/generic_sub_controller.cpp \
  sub_menu/language_controller.cpp \
  sub_menu/message_table_cell_with_editable_text_with_separator.cpp \
  sub_menu/preferences_controller.cpp \
	sub_menu/contributors_controller.cpp \
)

i18n_files += $(addprefix apps/settings/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

$(eval $(call depends_on_image,apps/settings/app.cpp,apps/settings/settings_icon.png))
