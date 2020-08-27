apps += Code::App
app_headers += apps/code/app.h

app_code_src = $(addprefix apps/code/,\
  app.cpp \
  console_controller.cpp \
  console_controller_default.cpp:-codeconsole \
  console_controller_locked.cpp:+codeconsole \
  console_edit_cell.cpp \
  console_line_cell.cpp \
  console_store.cpp \
  editor_controller.cpp \
  editor_view.cpp \
  helpers.cpp \
  menu_controller.cpp \
  menu_controller_default.cpp:-codeconsole \
  menu_controller_locked.cpp:+codeconsole \
  python_text_area.cpp \
  sandbox_controller.cpp \
  script_name_cell.cpp \
  script_parameter_controller.cpp \
)

app_code_test_src = $(addprefix apps/code/,\
  python_toolbox.cpp \
  script.cpp \
  script_node_cell.cpp \
  script_store.cpp \
  script_template.cpp \
  variable_box_empty_controller.cpp \
  variable_box_controller.cpp \
)

tests_src += $(addprefix apps/code/test/,\
  variable_box_controller.cpp\
)

app_code_src += $(app_code_test_src)
apps_src += $(app_code_src)

i18n_files += $(call i18n_with_universal_for,code/base)
i18n_files += $(call i18n_with_universal_for,code/catalog)
i18n_files += $(call i18n_with_universal_for,code/toolbox)

$(eval $(call depends_on_image,apps/code/app.cpp,apps/code/code_icon.png))
