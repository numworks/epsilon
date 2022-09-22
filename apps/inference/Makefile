apps += Inference::App
app_headers += apps/inference/app.h

app_inference_test_src = apps/inference/models/inference.cpp

app_inference_test_src += $(addprefix apps/inference/models/probability/distribution/,\
  binomial_distribution.cpp \
  chi_squared_distribution.cpp \
  distribution.cpp \
  fisher_distribution.cpp \
  geometric_distribution.cpp \
  hypergeometric_distribution.cpp \
  student_distribution.cpp \
  exponential_distribution.cpp \
  normal_distribution.cpp \
  poisson_distribution.cpp \
  uniform_distribution.cpp \
)

app_inference_src = $(addprefix apps/inference/,\
  app.cpp \
  text_helpers.cpp \
)

app_inference_test_src += apps/inference/text_helpers.cpp

app_inference_test_src += $(addprefix apps/inference/models/probability/calculation/,\
  calculation.cpp \
  discrete_calculation.cpp \
  left_integral_calculation.cpp \
  right_integral_calculation.cpp \
  finite_integral_calculation.cpp \
)

app_inference_test_src += $(addprefix apps/inference/models/statistic/,\
  interfaces/distributions.cpp \
  interfaces/significance_tests.cpp \
  chi2_test.cpp \
  goodness_test.cpp \
  homogeneity_test.cpp \
  interval.cpp \
  slope_t_interval.cpp \
  slope_t_statistic.cpp \
  slope_t_test.cpp \
  statistic.cpp \
  table.cpp \
  test.cpp \
  two_proportions_z_test.cpp \
)

app_inference_src += $(addprefix apps/inference/probability/,\
  calculation_cell.cpp \
  calculation_controller.cpp \
  calculation_popup_data_source.cpp \
  distribution_cell.cpp \
  distribution_controller.cpp \
  distribution_curve_view.cpp \
  parameters_controller.cpp \
)

app_inference_src += $(addprefix apps/inference/shared/,\
  dynamic_cells_data_source.cpp \
  expression_cell_with_editable_text_with_message.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/chi_square_and_slope/,\
  categorical_controller.cpp \
  categorical_table_cell.cpp \
  categorical_type_controller.cpp \
  goodness_table_cell.cpp \
  homogeneity_data_source.cpp \
  input_goodness_controller.cpp \
  input_homogeneity_controller.cpp \
  input_homogeneity_table_cell.cpp \
  input_slope_controller.cpp \
  result_homogeneity_table_cell.cpp \
  results_homogeneity_controller.cpp \
  slope_table_cell.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/interval/,\
  interval_conclusion_view.cpp \
  interval_curve_view.cpp \
  interval_graph_controller.cpp \
  interval_graph_view.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/test/,\
  hypothesis_controller.cpp \
  legend_view.cpp \
  test_curve_view.cpp \
  test_graph_controller.cpp \
  test_graph_view.cpp \
  zoom_hint_view.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/,\
  comparison_operator_popup_data_source.cpp \
  input_controller.cpp \
  message_table_cell_with_separator.cpp \
  results_controller.cpp \
  results_data_source.cpp \
  statistic_curve_view.cpp \
  test_controller.cpp \
  type_controller.cpp \
)

app_inference_src += $(app_inference_test_src)
apps_src += $(app_inference_src)

i18n_files += $(call i18n_with_universal_for,inference/i18n/base)

tests_src += $(addprefix apps/inference/test/,\
  distributions.cpp\
  statistics.cpp \
)

# Allow #include "inference/..."
sources = apps/apps_container.cpp apps/apps_container_storage.cpp apps/main.cpp $(app_inference_src) $(app_inference_test_src) $(tests_src)
$(call object_for,$(sources)): SFLAGS += -Iapps

# Image dependencies

$(eval $(call depends_on_image,apps/inference/app.cpp,apps/inference/inference_icon.png))

$(eval $(call depends_on_transparent_image,apps/inference/app.cpp,$(addprefix apps/inference/images/,\
  probability.png \
  confidence_interval.png \
  significance_test.png \
)))

$(eval $(call depends_on_transparent_image,apps/inference/probability/distribution_controller.cpp,$(addprefix apps/inference/images/,\
  binomial_icon.png \
  chi_squared_icon.png \
  exponential_icon.png \
  fisher_icon.png \
  geometric_icon.png \
  hypergeometric_icon.png \
  normal_icon.png \
  poisson_icon.png \
  student_icon.png \
  uniform_icon.png \
)))

$(eval $(call depends_on_transparent_image,apps/inference/probability/calculation_controller.cpp \
 apps/inference/probability/calculation_popup_data_source.cpp,$(addprefix apps/inference/images/,\
  calculation1_icon.png \
  calculation2_icon.png \
  calculation3_icon.png \
  calculation4_icon.png \
)))
