apps += Graph::App
app_headers += apps/graph/app.h

app_graph_src = $(addprefix apps/graph/,\
  app.cpp \
  continuous_function_store.cpp \
  graph/banner_view.cpp \
  graph/calculation_graph_controller.cpp \
  graph/calculation_parameter_controller.cpp \
  graph/curve_parameter_controller.cpp \
  graph/extremum_graph_controller.cpp \
  graph/graph_controller.cpp \
  graph/graph_controller_helper.cpp \
  graph/graph_view.cpp \
  graph/preimage_graph_controller.cpp\
  graph/preimage_parameter_controller.cpp\
  graph/integral_graph_controller.cpp \
  graph/intersection_graph_controller.cpp \
  graph/root_graph_controller.cpp \
  graph/tangent_graph_controller.cpp \
  list/domain_parameter_controller.cpp \
  list/list_controller.cpp \
  list/list_parameter_controller.cpp \
  list/text_field_function_title_cell.cpp \
  list/type_helper.cpp \
  list/type_parameter_controller.cpp \
  values/abscissa_title_cell.cpp \
  values/derivative_parameter_controller.cpp \
  values/function_parameter_controller.cpp \
  values/interval_parameter_selector_controller.cpp \
  values/values_controller.cpp \
)

apps_src += $(app_graph_src)

i18n_files += $(addprefix apps/graph/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

$(eval $(call depends_on_image,apps/graph/app.cpp,apps/graph/graph_icon.png))
