apps += Code::App
app_headers += apps/code/app.h

app_code_test_src = $(addprefix apps/code/,\
  script_template.cpp \
)

app_code_src = $(addprefix apps/code/,\
  app.cpp \
  console_controller.cpp \
  console_edit_cell.cpp \
  console_line_cell.cpp \
  console_store.cpp \
  editor_controller.cpp \
  editor_view.cpp \
  helpers.cpp \
  menu_controller.cpp \
  python_toolbox.cpp \
  python_text_area.cpp \
  sandbox_controller.cpp \
  script.cpp \
  script_name_cell.cpp \
  script_node_cell.cpp \
  script_parameter_controller.cpp \
  script_store.cpp \
  variable_box_controller.cpp \
)

app_code_src += $(app_code_test_src)
apps_src += $(app_code_src)

i18n_files += $(addprefix apps/code/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
  base.universal.i18n\
  catalog.de.i18n\
  catalog.en.i18n\
  catalog.es.i18n\
  catalog.fr.i18n\
  catalog.pt.i18n\
  catalog.universal.i18n\
  toolbox.de.i18n\
  toolbox.en.i18n\
  toolbox.es.i18n\
  toolbox.fr.i18n\
  toolbox.pt.i18n\
  toolbox.universal.i18n\
)

$(eval $(call depends_on_image,apps/code/app.cpp,apps/code/code_icon.png))
