SOURCES_apps += $(addprefix $(PATH_apps)/inference/, \
  app.cpp \
$(addprefix models/statistic/, \
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
) \
$(addprefix shared/, \
  dynamic_cells_data_source.cpp \
) \
$(addprefix statistic/chi_square/, \
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
) \
$(addprefix statistic/interval/, \
  interval_conclusion_view.cpp \
  interval_curve_view.cpp \
  interval_graph_controller.cpp \
  interval_graph_view.cpp \
) \
$(addprefix statistic/store/, \
  input_store_controller.cpp \
  store_table_cell.cpp \
) \
$(addprefix statistic/test/, \
  hypothesis_controller.cpp \
  legend_view.cpp \
  test_curve_view.cpp \
  test_graph_controller.cpp \
  test_graph_view.cpp \
  zoom_hint_view.cpp \
) \
$(addprefix statistic/, \
  categorical_cell.cpp \
  categorical_controller.cpp \
  categorical_table_cell.cpp \
  comparison_operator_popup_data_source.cpp \
  dataset_controller.cpp \
  input_controller.cpp \
  results_controller.cpp \
  test_controller.cpp \
  type_controller.cpp \
) \
$(addprefix test/, \
  statistics.cpp:+test \
) \
  inference_icon.png \
$(addprefix images/, \
  confidence_interval.png \
  significance_test.png \
) \
)

$(call depends_on_image,$(PATH_apps)/inference/app.cpp,$(PATH_apps)/inference/inference_icon.png)

$(call depends_on_transparent_image,$(PATH_apps)/inference/app.cpp,$(addprefix $(PATH_apps)/inference/images/,\
  confidence_interval.png \
  significance_test.png \
))

$(call i18n_with_universal,inference/i18n/base)
