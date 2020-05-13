apps += Regression::App
app_headers += apps/regression/app.h

app_regression_test_src += $(addprefix apps/regression/,\
  linear_model_helper.cpp \
  regression_context.cpp \
  store.cpp \
)

app_regression_test_src += $(addprefix apps/regression/model/,\
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
  initialisation_parameter_controller.cpp \
  regression_controller.cpp \
  store_controller.cpp \
  store_parameter_controller.cpp \
)

app_regression_src += $(app_regression_test_src)
apps_src += $(app_regression_src)

i18n_files += $(addprefix apps/regression/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
  base.universal.i18n\
)

tests_src += $(addprefix apps/regression/test/,\
  model.cpp\
)

$(eval $(call depends_on_image,apps/regression/app.cpp,apps/regression/regression_icon.png))
