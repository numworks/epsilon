apps += Probability::App
app_headers += apps/probability/app.h

app_probability_test_src = $(addprefix apps/probability/models/distribution/,\
  binomial_distribution.cpp \
  chi_squared_distribution.cpp \
  fisher_distribution.cpp \
  geometric_distribution.cpp \
  helper.cpp \
  hypergeometric_function.cpp\
  distribution.cpp \
  regularized_gamma.cpp \
  student_distribution.cpp \
  two_parameter_distribution.cpp \
)

app_probability_src = $(addprefix apps/probability/,\
  app.cpp \
  text_helpers.cpp \
)

app_probability_src += $(addprefix apps/probability/models/distribution/,\
  exponential_distribution.cpp \
  normal_distribution.cpp \
  poisson_distribution.cpp \
  regularized_gamma.cpp \
  uniform_distribution.cpp \
)

app_probability_src += $(addprefix apps/probability/models/calculation/,\
	calculation.cpp \
  discrete_calculation.cpp \
  left_integral_calculation.cpp \
  right_integral_calculation.cpp \
  finite_integral_calculation.cpp \
)

app_probability_src += $(addprefix apps/probability/models/statistic/,\
	z_statistic.cpp \
	t_statistic.cpp \
	one_proportion_statistic.cpp \
	two_proportions_statistic.cpp \
	one_mean_z_statistic.cpp \
	one_mean_t_statistic.cpp \
)

app_probability_src += $(addprefix apps/probability/gui/,\
  calculation_cell.cpp \
  cell.cpp \
  distribution_curve_view.cpp \
	image_cell.cpp \
  responder_image_cell.cpp \
	input_table_view.cpp \
	vertical_layout.cpp \
	results_data_source.cpp \
)

app_probability_src += $(addprefix apps/probability/controllers/,\
	calculation_controller.cpp \
  distribution_controller.cpp \
  parameters_controller.cpp \
	calculation_type_controller.cpp \
	menu_controller.cpp \
	test_controller.cpp \
	hypothesis_controller.cpp \
	categorical_type_controller.cpp \
	type_controller.cpp \
	input_controller.cpp \
	results_controller.cpp \
	input_goodness_controller.cpp \
	input_homogeneity_controller.cpp \
	homogeneity_results_controller.cpp \
)

app_probability_src +=  $(addprefix apps/probability/abstract/,\
  homogeneity_data_source.cpp \
)


app_probability_src += $(app_probability_test_src)
apps_src += $(app_probability_src)

i18n_files += $(call i18n_with_universal_for,probability/translations/base)

tests_src += $(addprefix apps/probability/test/,\
  hypergeometric_function.cpp\
  distributions.cpp\
  regularized_gamma.cpp \
)

# Allow #include "probability/..."
$(call object_for,apps/apps_container.cpp apps/apps_container_storage.cpp apps/main.cpp) $(call object_for,$(app_probability_src)): SFLAGS += -Iapps

# Image dependencies

$(eval $(call depends_on_image,apps/probability/app.cpp,apps/probability/probability_icon.png))

$(eval $(call depends_on_image,apps/probability/controllers/distribution_controller.cpp,$(addprefix apps/probability/images/,\
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

$(eval $(call depends_on_image,$(addprefix apps/probability/controllers/,calculation_type_controller.cpp calculation_controller.cpp),$(addprefix apps/probability/images/,\
  calcul1_icon.png \
  calcul2_icon.png \
  calcul3_icon.png \
  calcul4_icon.png \
  focused_calcul1_icon.png \
  focused_calcul2_icon.png \
  focused_calcul3_icon.png \
  focused_calcul4_icon.png \
)))
