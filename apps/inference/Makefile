apps += Inference::App
app_headers += apps/inference/app.h

app_inference_test_src = apps/inference/models/inference.cpp

app_inference_src = $(addprefix apps/inference/,\
  app.cpp \
  text_helpers.cpp \
)

app_inference_test_src += apps/inference/text_helpers.cpp

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
  statistics.cpp \
)

# Allow #include "inference/..."
sources = apps/apps_container.cpp apps/apps_container_storage.cpp apps/main.cpp $(app_inference_src) $(app_inference_test_src) $(tests_src)
$(call object_for,$(sources)): SFLAGS += -Iapps

# Image dependencies

$(eval $(call depends_on_image,apps/inference/app.cpp,apps/inference/inference_icon.png))

$(eval $(call depends_on_transparent_image,apps/inference/app.cpp,$(addprefix apps/inference/images/,\
  confidence_interval.png \
  significance_test.png \
)))
