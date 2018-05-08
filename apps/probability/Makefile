snapshots += Probability::App::Snapshot
snapshot_headers += apps/probability/app.h

app_objs += $(addprefix apps/probability/,\
  app.o\
  calculation/calculation.o\
  calculation/discrete_calculation.o\
  calculation/left_integral_calculation.o\
  calculation/right_integral_calculation.o\
  calculation/finite_integral_calculation.o\
  calculation_controller.o\
  calculation_cell.o\
  calculation_type_controller.o\
  cell.o\
  image_cell.o\
  law/binomial_law.o\
  law/erf_inv.o\
  law/exponential_law.o\
  law/law.o\
  law/normal_law.o\
  law/one_parameter_law.o\
  law/poisson_law.o\
  law/two_parameter_law.o\
  law/uniform_law.o\
  law_controller.o\
  law_curve_view.o\
  parameters_controller.o\
  responder_image_cell.o\
)

i18n_files += $(addprefix apps/probability/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests += $(addprefix apps/probability/test/,\
  erf_inv.cpp\
)
test_objs += $(addprefix apps/probability/law/, erf_inv.o)

app_images += apps/probability/probability_icon.png

app_images += $(addprefix apps/probability/images/,\
  binomial_icon.png\
  calcul1_icon.png\
  calcul2_icon.png\
  calcul3_icon.png\
  calcul4_icon.png\
  exponential_icon.png\
  focused_binomial_icon.png\
  focused_calcul1_icon.png\
  focused_calcul2_icon.png\
  focused_calcul3_icon.png\
  focused_calcul4_icon.png\
  focused_exponential_icon.png\
  focused_normal_icon.png\
  focused_poisson_icon.png\
  focused_uniform_icon.png\
  normal_icon.png\
  poisson_icon.png\
  uniform_icon.png\
)
