SOURCES_apps += $(addprefix $(PATH_apps)/inference/,$(addsuffix :-test, \
  app.cpp \
  statistic/dynamic_cells_data_source.cpp \
  statistic/chi_square/categorical_type_controller.cpp \
  statistic/chi_square/homogeneity_data_source.cpp \
  statistic/chi_square/input_goodness_controller.cpp \
  statistic/chi_square/input_goodness_table_cell.cpp \
  statistic/chi_square/input_homogeneity_controller.cpp \
  statistic/chi_square/input_homogeneity_table_cell.cpp \
  statistic/chi_square/results_goodness_controller.cpp \
  statistic/chi_square/results_goodness_table_cell.cpp \
  statistic/chi_square/results_homogeneity_controller.cpp \
  statistic/chi_square/results_homogeneity_table_cell.cpp \
  statistic/interval/interval_conclusion_view.cpp \
  statistic/interval/interval_curve_view.cpp \
  statistic/interval/interval_graph_controller.cpp \
  statistic/interval/interval_graph_view.cpp \
  statistic/store/input_store_controller.cpp \
  statistic/store/store_table_cell.cpp \
  statistic/test/hypothesis_controller.cpp \
  statistic/test/legend_view.cpp \
  statistic/test/test_curve_view.cpp \
  statistic/test/test_graph_controller.cpp \
  statistic/test/test_graph_view.cpp \
  statistic/test/zoom_hint_view.cpp \
  statistic/categorical_cell.cpp \
  statistic/categorical_controller.cpp \
  statistic/categorical_table_cell.cpp \
  statistic/comparison_operator_popup_data_source.cpp \
  statistic/dataset_controller.cpp \
  statistic/input_controller.cpp \
  statistic/results_controller.cpp \
  statistic/test_controller.cpp \
  statistic/type_controller.cpp \
  inference_icon.png \
  images/confidence_interval.png \
  images/significance_test.png \
) \
  models/chi2_test.cpp \
  models/goodness_test.cpp \
  models/homogeneity_test.cpp \
  models/interval.cpp \
  models/one_mean_statistic.cpp \
  models/slope_t_statistic.cpp \
  models/statistic.cpp \
  models/table.cpp \
  models/table_from_store.cpp \
  models/test.cpp \
  models/two_means_statistic.cpp \
  test/statistics.cpp:+test \
)

$(call depends_on_image,$(PATH_apps)/inference/app.cpp,$(PATH_apps)/inference/inference_icon.png)

$(call depends_on_transparent_image,$(PATH_apps)/inference/app.cpp,$(addprefix $(PATH_apps)/inference/images/,\
  confidence_interval.png \
  significance_test.png \
))

$(call i18n_with_universal,inference/i18n/base)
