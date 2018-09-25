apps += Settings::App
app_headers += apps/settings/app.h

app_objs += $(addprefix apps/settings/,\
  app.o\
  main_controller.o\
  settings_message_tree.o\
  sub_menu/about_controller.o\
  sub_menu/display_mode_controller.o\
  sub_menu/exam_mode_controller.o\
  sub_menu/generic_sub_controller.o\
  sub_menu/language_controller.o\
  sub_menu/message_table_cell_with_editable_text_with_separator.o\
  sub_menu/preferences_controller.o\
)

i18n_files += $(addprefix apps/settings/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

app_images += apps/settings/settings_icon.png
