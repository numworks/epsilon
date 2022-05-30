apps += Regression::App
app_headers += apps/regression/app.h

app_regression_test_src += $(addprefix apps/regression/,\
  store.cpp \
)

app_regression_test_src += $(addprefix apps/regression/model/,\
  affine_model.cpp \
  cubic_model.cpp \
  exponential_model.cpp \
  linear_model.cpp \
  logarithmic_model.cpp \
  logistic_model.cpp \
  model.cpp \
  power_model.cpp \
  proportional_model.cpp \
  quadratic_model.cpp \
  quartic_model.cpp \
  trigonometric_model.cpp \
  median_model.cpp \
)

app_regression_src = $(addprefix apps/regression/,\
  app.cpp \
  banner_view.cpp \
  calculation_controller.cpp \
  column_title_cell.cpp \
  even_odd_double_buffer_text_cell_with_separator.cpp \
  graph_controller.cpp \
  graph_options_controller.cpp \
  graph_view.cpp \
  go_to_parameter_controller.cpp \
  regression_controller.cpp \
  residual_plot_controller.cpp \
  residual_plot_curve_view.cpp \
  residual_plot_range.cpp \
  store_controller.cpp \
  store_parameter_controller.cpp \
)

app_regression_src += $(app_regression_test_src)
apps_src += $(app_regression_src)

i18n_files += $(call i18n_with_universal_for,regression/base)

tests_src += $(addprefix apps/regression/test/,\
  model.cpp\
)

$(eval $(call depends_on_image,apps/regression/app.cpp,apps/regression/regression_icon.png))
