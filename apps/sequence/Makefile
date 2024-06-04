SOURCES_apps += $(addprefix $(PATH_apps)/sequence/, \
  app.cpp \
$(addprefix graph/, \
  cobweb_controller.cpp \
  cobweb_graph_view.cpp \
  curve_parameter_controller.cpp \
  curve_view_range.cpp \
  go_to_parameter_controller.cpp \
  graph_controller.cpp \
  graph_view.cpp \
  term_sum_controller.cpp \
) \
$(addprefix list/, \
  editable_sequence_cell.cpp \
  list_controller.cpp \
  list_parameter_controller.cpp \
  sequence_cell.cpp \
  sequence_toolbox_data_source.cpp \
  type_parameter_controller.cpp \
  vertical_sequence_title_cell.cpp \
) \
$(addprefix values/, \
  interval_parameter_controller.cpp \
  sequence_column_parameter_controller.cpp \
  sum_column_parameter_controller.cpp \
  values_controller.cpp \
) \
$(addprefix test/, \
  sequence.cpp:+test \
) \
  sequence_icon.png \
)

$(call depends_on_image,$(PATH_apps)/sequence/app.cpp,$(PATH_apps)/sequence/sequence_icon.png)

$(call i18n_without_universal,sequence/i18n/base)
