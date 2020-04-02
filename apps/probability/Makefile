apps += Probability::App
app_headers += apps/probability/app.h

app_probability_test_src = $(addprefix apps/probability/,\
  distribution/binomial_distribution.cpp \
  distribution/chi_squared_distribution.cpp \
  distribution/fisher_distribution.cpp \
  distribution/geometric_distribution.cpp \
  distribution/helper.cpp \
  distribution/hypergeometric_function.cpp\
  distribution/distribution.cpp \
  distribution/regularized_gamma.cpp \
  distribution/student_distribution.cpp \
  distribution/two_parameter_distribution.cpp \
)

app_probability_src = $(addprefix apps/probability/,\
  app.cpp \
  calculation/calculation.cpp \
  calculation/discrete_calculation.cpp \
  calculation/left_integral_calculation.cpp \
  calculation/right_integral_calculation.cpp \
  calculation/finite_integral_calculation.cpp \
  calculation_controller.cpp \
  calculation_cell.cpp \
  calculation_type_controller.cpp \
  cell.cpp \
  image_cell.cpp \
  distribution/exponential_distribution.cpp \
  distribution/normal_distribution.cpp \
  distribution/poisson_distribution.cpp \
  distribution/regularized_gamma.cpp \
  distribution/uniform_distribution.cpp \
  distribution_controller.cpp \
  distribution_curve_view.cpp \
  parameters_controller.cpp \
  responder_image_cell.cpp \
)

app_probability_src += $(app_probability_test_src)
apps_src += $(app_probability_src)

i18n_files += $(addprefix apps/probability/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
  base.universal.i18n\
)

tests_src += $(addprefix apps/probability/test/,\
  hypergeometric_function.cpp\
  distributions.cpp\
  regularized_gamma.cpp \
)

# Image dependencies

$(eval $(call depends_on_image,apps/probability/app.cpp,apps/probability/probability_icon.png))

$(eval $(call depends_on_image,apps/probability/distribution_controller.cpp,$(addprefix apps/probability/images/, \
  binomial_icon.png \
  chi_squared_icon.png \
  exponential_icon.png \
  fisher_icon.png \
  focused_binomial_icon.png \
  focused_chi_squared_icon.png \
  focused_exponential_icon.png \
  focused_fisher_icon.png \
  focused_geometric_icon.png \
  focused_normal_icon.png \
  focused_poisson_icon.png \
  focused_student_icon.png \
  focused_uniform_icon.png \
  geometric_icon.png \
  normal_icon.png \
  poisson_icon.png \
  student_icon.png \
  uniform_icon.png \
)))

$(eval $(call depends_on_image,$(addprefix apps/probability/,calculation_type_controller.cpp calculation_controller.cpp),$(addprefix apps/probability/images/, \
  calcul1_icon.png \
  calcul2_icon.png \
  calcul3_icon.png \
  calcul4_icon.png \
  focused_calcul1_icon.png \
  focused_calcul2_icon.png \
  focused_calcul3_icon.png \
  focused_calcul4_icon.png \
)))
