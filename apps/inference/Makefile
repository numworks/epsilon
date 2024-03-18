apps += Inference::App
app_headers += apps/inference/app.h

app_inference_src = $(addprefix apps/inference/,\
  app.cpp \
)

app_inference_test_src += $(addprefix apps/inference/models/statistic/,\
  interfaces/distributions.cpp \
  interfaces/significance_tests.cpp \
  chi2_test.cpp \
  goodness_test.cpp \
  homogeneity_test.cpp \
  interval.cpp \
  one_mean_statistic.cpp \
  raw_data_statistic.cpp \
  slope_t_interval.cpp \
  slope_t_statistic.cpp \
  slope_t_test.cpp \
  statistic.cpp \
  table.cpp \
  test.cpp \
  two_means_statistic.cpp \
  two_proportions_z_test.cpp \
)

app_inference_src += $(addprefix apps/inference/shared/,\
  dynamic_cells_data_source.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/chi_square/,\
  categorical_type_controller.cpp \
  homogeneity_data_source.cpp \
  input_goodness_controller.cpp \
  input_goodness_table_cell.cpp \
  input_homogeneity_controller.cpp \
  input_homogeneity_table_cell.cpp \
  results_goodness_controller.cpp \
  results_goodness_table_cell.cpp \
  results_homogeneity_controller.cpp \
  results_homogeneity_table_cell.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/interval/,\
  interval_conclusion_view.cpp \
  interval_curve_view.cpp \
  interval_graph_controller.cpp \
  interval_graph_view.cpp \
)

app_inference_src += $(addprefix apps/inference/statistic/store/,\
  input_store_controller.cpp \
  store_table_cell.cpp \
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
  categorical_cell.cpp \
  categorical_controller.cpp \
  categorical_table_cell.cpp \
  comparison_operator_popup_data_source.cpp \
  dataset_controller.cpp \
  input_controller.cpp \
  results_controller.cpp \
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
