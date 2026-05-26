SOURCES_apps += $(addprefix $(PATH_apps)/elements/,$(addsuffix :-test, \
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
))

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/elements/app.cpp,$(PATH_apps)/icons/light/elements_light_icon.png,$(PATH_apps)/icons/dark/elements_dark_icon.png,:-test)

$(call i18n_with_universal,elements/i18n/base)
