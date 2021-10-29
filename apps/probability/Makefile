apps += Probability::App
app_headers += apps/probability/app.h

app_probability_test_src = $(addprefix apps/probability/models/distribution/,\
  binomial_distribution.cpp \
  chi_squared_distribution.cpp \
  distribution.cpp \
  fisher_distribution.cpp \
  geometric_distribution.cpp \
  helper.cpp \
  hypergeometric_function.cpp\
  regularized_gamma.cpp \
  student_distribution.cpp \
  two_parameter_distribution.cpp \
  exponential_distribution.cpp \
  normal_distribution.cpp \
  poisson_distribution.cpp \
  regularized_gamma.cpp \
  uniform_distribution.cpp \
)

app_probability_test_src += $(addprefix apps/probability/models/,\
  student_law.cpp \
  chi2_law.cpp \
)

app_probability_src = $(addprefix apps/probability/,\
  app.cpp \
  text_helpers.cpp \
)

app_probability_test_src += apps/probability/text_helpers.cpp

app_probability_src += $(addprefix apps/probability/models/,\
  statistic_view_range.cpp \
  student_law.cpp \
  chi2_law.cpp \
  results_data_source.cpp \
)

app_probability_src += $(addprefix apps/probability/models/calculation/,\
  calculation.cpp \
  discrete_calculation.cpp \
  left_integral_calculation.cpp \
  right_integral_calculation.cpp \
  finite_integral_calculation.cpp \
)

app_probability_test_src += $(addprefix apps/probability/models/statistic/,\
  statistic.cpp \
  z_statistic.cpp \
  t_statistic.cpp \
  one_proportion_statistic.cpp \
  two_proportions_statistic.cpp \
  one_mean_z_statistic.cpp \
  one_mean_t_statistic.cpp \
  two_means_z_statistic.cpp \
  two_means_t_statistic.cpp \
  pooled_two_means_statistic.cpp \
  chi2_statistic.cpp \
  goodness_statistic.cpp \
  homogeneity_statistic.cpp \
)

app_probability_src += $(addprefix apps/probability/gui/,\
  bordering_view.cpp \
  calculation_cell.cpp \
  calculation_popup_data_source.cpp \
  centering_view.cpp \
  comparison_operator_popup_data_source.cpp \
  distribution_cell.cpp \
  distribution_curve_view.cpp \
  dropdown_view.cpp \
  expression_cell_with_editable_text_with_message.cpp \
  highlight_image_cell.cpp \
  input_goodness_table_view.cpp \
  interval_conclusion_view.cpp \
  legend_view.cpp \
  message_table_cell_with_separator.cpp \
  results_homogeneity_view.cpp \
  selectable_table_view_with_background.cpp \
  statistic_curve_view.cpp \
  statistic_graph_view.cpp \
  subapp_cell.cpp \
  test_conclusion_view.cpp \
)

app_probability_src += $(addprefix apps/probability/controllers/,\
  calculation_controller.cpp \
  categorical_type_controller.cpp \
  distribution_controller.cpp \
  goodness_table_view_controller.cpp \
  homogeneity_table_view_controller.cpp \
  hypothesis_controller.cpp \
  input_controller.cpp \
  input_goodness_controller.cpp \
  input_homogeneity_controller.cpp \
  menu_controller.cpp \
  parameters_controller.cpp \
  results_controller.cpp \
  results_homogeneity_controller.cpp \
  statistic_graph_controller.cpp \
  test_controller.cpp \
  type_controller.cpp \
)

app_probability_src +=  $(addprefix apps/probability/abstract/,\
  dynamic_cells_data_source.cpp \
  homogeneity_data_source.cpp \
  input_categorical_controller.cpp \
  input_categorical_view.cpp \
  input_homogeneity_data_source.cpp \
  results_homogeneity_data_source.cpp \
  table_view_controller.cpp \
)


app_probability_src += $(app_probability_test_src)
apps_src += $(app_probability_src)

i18n_files += $(call i18n_with_universal_for,probability/translations/base)

tests_src += $(addprefix apps/probability/test/,\
  hypergeometric_function.cpp\
  distributions.cpp\
  regularized_gamma.cpp \
  student_law.cpp \
  chi2_law.cpp \
  statistics.cpp \
  data.cpp \
)

# Allow #include "probability/..."
sources = apps/apps_container.cpp apps/apps_container_storage.cpp apps/main.cpp $(app_probability_src) $(app_probability_test_src) $(tests_src)
$(call object_for,$(sources)): SFLAGS += -Iapps

# Image dependencies

$(eval $(call depends_on_image,apps/probability/app.cpp,apps/probability/probability_icon.png))

$(eval $(call depends_on_transparent_image,apps/probability/controllers/menu_controller.cpp,$(addprefix apps/probability/images/,\
  probability.png \
  confidence_interval.png \
  significance_test.png \
)))

$(eval $(call depends_on_transparent_image,apps/probability/controllers/distribution_controller.cpp,$(addprefix apps/probability/images/,\
  binomial_icon.png \
  chi_squared_icon.png \
  exponential_icon.png \
  fisher_icon.png \
  geometric_icon.png \
  normal_icon.png \
  poisson_icon.png \
  student_icon.png \
  uniform_icon.png \
)))

$(eval $(call depends_on_transparent_image,apps/probability/controllers/calculation_controller.cpp \
 apps/probability/gui/calculation_popup_data_source.cpp,$(addprefix apps/probability/images/,\
  calculation1_icon.png \
  calculation2_icon.png \
  calculation3_icon.png \
  calculation4_icon.png \
)))

$(eval $(call depends_on_transparent_image,apps/probability/gui/test_conclusion_view.cpp,$(addprefix apps/probability/images/,\
  test_success.png \
)))

$(eval $(call depends_on_transparent_image,apps/probability/gui/dropdown_view.cpp,\
  apps/probability/images/caret.png \
))
