apps += Graph::App
app_headers += apps/graph/app.h

app_graph_test_src = $(addprefix apps/graph/,\
  continuous_function_store.cpp \
)

app_graph_src = $(addprefix apps/graph/,\
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
  list/function_cell.cpp \
  list/function_models_parameter_controller.cpp \
  list/function_toolbox.cpp \
  list/list_controller.cpp \
  shared/details_parameter_controller.cpp \
  shared/domain_parameter_controller.cpp \
  shared/function_parameter_controller.cpp \
  values/abscissa_title_cell.cpp \
  values/derivative_parameter_controller.cpp \
  values/function_column_parameter_controller.cpp \
  values/interval_parameter_selector_controller.cpp \
  values/values_controller.cpp \
)

app_graph_src += $(app_graph_test_src)
apps_src += $(app_graph_src)

i18n_files += $(call i18n_without_universal_for,graph/base)

tests_src += $(addprefix apps/graph/test/,\
  caching.cpp \
  helper.cpp \
  function_properties.cpp \
)

$(eval $(call depends_on_image,apps/graph/app.cpp,apps/graph/graph_icon.png))
