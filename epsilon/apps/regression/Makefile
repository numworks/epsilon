SOURCES_apps += $(addprefix $(PATH_apps)/regression/,$(addsuffix :-test, \
  app.cpp \
  stats/calculation_controller.cpp \
  stats/column_title_cell.cpp \
  stats/even_odd_double_buffer_text_cell.cpp \
  graph/banner_view.cpp \
  graph/go_to_parameter_controller.cpp \
  graph/graph_controller.cpp \
  graph/graph_options_controller.cpp \
  graph/graph_view.cpp \
  graph/regression_controller.cpp \
  graph/residual_plot_controller.cpp \
  graph/residual_plot_curve_view.cpp \
  graph/residual_plot_range.cpp \
  data/store_controller.cpp \
  data/store_parameter_controller.cpp \
) \
  model.cpp \
  store.cpp \
  test/model.cpp:+test \
)

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/regression/app.cpp,$(PATH_apps)/icons/light/regression_light_icon.png,$(PATH_apps)/icons/dark/regression_dark_icon.png,:-test)

$(call i18n_with_universal,regression/i18n/base)
