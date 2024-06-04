SOURCES_apps += $(addprefix $(PATH_apps)/graph/, \
  app.cpp \
  graph/area_between_curves_graph_controller.cpp \
  graph/area_between_curves_parameter_controller.cpp \
  graph/banner_view.cpp \
  graph/calculation_graph_controller.cpp \
  graph/calculation_parameter_controller.cpp \
  graph/curve_parameter_controller.cpp \
  graph/extremum_graph_controller.cpp \
  graph/graph_controller.cpp \
  graph/graph_controller_helper.cpp \
  graph/graph_view.cpp \
  graph/integral_graph_controller.cpp \
  graph/interest_view.cpp \
  graph/intersection_graph_controller.cpp \
  graph/points_of_interest_cache.cpp \
  graph/preimage_graph_controller.cpp\
  graph/preimage_parameter_controller.cpp\
  graph/root_graph_controller.cpp \
  graph/tangent_graph_controller.cpp \
  list/editable_function_cell.cpp \
  list/function_cell.cpp \
  list/function_models_parameter_controller.cpp \
  list/list_controller.cpp \
  shared/derivative_column_parameter_controller.cpp \
  shared/derivatives_parameter_controller.cpp \
  shared/details_parameter_controller.cpp \
  shared/domain_parameter_controller.cpp \
  shared/function_parameter_controller.cpp \
  values/interval_parameter_selector_controller.cpp \
  values/values_controller.cpp \
  test/caching.cpp:+test \
  test/helper.cpp:+test \
  test/function_names.cpp:+test \
  test/function_properties.cpp:+test \
  graph_icon.png \
)

$(call depends_on_image,apps/graph/app.cpp,apps/graph/graph_icon.png)

$(call i18n_without_universal,graph/i18n/base)
