apps += Elements::App
app_headers += apps/elements/app.h

app_elements_src = $(addprefix apps/elements/,\
  app.cpp \
  banner_view.cpp \
  data_field.cpp \
  details_list_controller.cpp \
  display_type_controller.cpp \
  elements_data_base.cpp \
  elements_view.cpp \
  elements_view_data_source.cpp \
  main_controller.cpp \
  single_element_view.cpp \
  suggestion_text_field.cpp \
  table_layout.cpp \
)

apps_src += $(app_elements_src)

i18n_files += $(call i18n_with_universal_for,elements/base)

$(eval $(call depends_on_image,apps/elements/app.cpp,apps/elements/elements_icon.png))
