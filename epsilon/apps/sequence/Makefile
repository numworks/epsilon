SOURCES_apps += $(addprefix $(PATH_apps)/sequence/,$(addsuffix :-test, \
  app.cpp \
  graph/cobweb_controller.cpp \
  graph/cobweb_graph_view.cpp \
  graph/curve_parameter_controller.cpp \
  graph/curve_view_range.cpp \
  graph/go_to_parameter_controller.cpp \
  graph/graph_controller.cpp \
  graph/graph_view.cpp \
  graph/term_sum_controller.cpp \
  list/editable_sequence_cell.cpp \
  list/list_controller.cpp \
  list/list_parameter_controller.cpp \
  list/sequence_cell.cpp \
  list/sequence_toolbox_data_source.cpp \
  list/type_parameter_controller.cpp \
  values/interval_parameter_controller.cpp \
  values/sequence_column_parameter_controller.cpp \
  values/sum_column_parameter_controller.cpp \
  values/values_controller.cpp \
  sequence_icon.png \
) \
  test/sequence.cpp:+test \
)

$(call depends_on_image,$(PATH_apps)/sequence/app.cpp,$(PATH_apps)/sequence/sequence_icon.png)

$(call i18n_without_universal,sequence/i18n/base)
