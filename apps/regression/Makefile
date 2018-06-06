snapshots += Regression::App::Snapshot
snapshot_headers += apps/regression/app.h

app_objs += $(addprefix apps/regression/,\
  app.o\
  banner_view.o\
  calculation_controller.o\
  column_title_cell.o\
  even_odd_buffer_text_cell_with_margin.o\
  even_odd_double_buffer_text_cell_with_separator.o\
  go_to_parameter_controller.o\
  graph_controller.o\
  graph_view.o\
  initialisation_parameter_controller.o\
  levenberg_marquardt.o\
  prediction_parameter_controller.o\
  regression_context.o\
  store.o\
  store_controller.o\
  store_parameter_controller.o\
)

app_objs += $(addprefix apps/regression/model/,\
  cubic_model.o\
  exponential_model.o\
  linear_model.o\
  logarithmic_model.o\
  logistic_model.o\
  power_model.o\
  quadratic_model.o\
  quartic_model.o\
  trigonometric_model.o\
)

i18n_files += $(addprefix apps/regression/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

app_images += apps/regression/regression_icon.png
