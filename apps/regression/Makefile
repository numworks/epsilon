SOURCES_apps += $(addprefix $(PATH_apps)/regression/, \
  app.cpp \
  store.cpp \
  model.cpp \
$(addprefix stats/, \
  calculation_controller.cpp \
  column_title_cell.cpp \
  even_odd_double_buffer_text_cell.cpp \
) \
$(addprefix graph/, \
  banner_view.cpp \
  go_to_parameter_controller.cpp \
  graph_controller.cpp \
  graph_options_controller.cpp \
  graph_view.cpp \
  regression_controller.cpp \
  residual_plot_controller.cpp \
  residual_plot_curve_view.cpp \
  residual_plot_range.cpp \
) \
$(addprefix data/, \
  store_controller.cpp \
  store_parameter_controller.cpp \
) \
$(addprefix test/, \
  model.cpp:+test \
) \
)

$(call depends_on_image,$(PATH_apps)/regression/app.cpp,$(PATH_apps)/regression/regression_icon.png)

$(call i18n_with_universal,regression/i18n/base)
