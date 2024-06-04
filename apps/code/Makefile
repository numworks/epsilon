SOURCES_apps += $(addprefix $(PATH_apps)/code/, \
  app.cpp \
  clipboard.cpp \
  console_controller.cpp \
  console_edit_cell.cpp \
  console_line_cell.cpp \
  console_store.cpp \
  editor_controller.cpp \
  editor_view.cpp \
  helpers.cpp \
  menu_controller.cpp \
  python_text_area.cpp \
  python_toolbox_controller.cpp \
  python_variable_box_controller.cpp \
  sandbox_controller.cpp \
  script.cpp \
  script_name_cell.cpp \
  script_parameter_controller.cpp \
  script_store.cpp \
  script_template.cpp \
  subtitle_cell.cpp \
  test/clipboard.cpp:+test \
  test/python_variable_box.cpp:+test \
  test/templates.cpp:+test \
  code_icon.png \
)

$(call depends_on_image,$(PATH_apps)/code/app.cpp,$(PATH_apps)/code/code_icon.png)

$(call i18n_with_universal,code/i18n/base)
$(call i18n_with_universal,code/i18n/catalog)
$(call i18n_with_universal,code/i18n/toolbox)
