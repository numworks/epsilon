SOURCES_apps += $(addprefix $(PATH_apps)/inference/,$(addsuffix :-test, \
  app.cpp \
  controllers/chi_square/categorical_type_controller.cpp \
  controllers/chi_square/input_goodness_controller.cpp \
  controllers/chi_square/input_homogeneity_controller.cpp \
  controllers/chi_square/results_goodness_controller.cpp \
  controllers/chi_square/results_homogeneity_controller.cpp \
  controllers/confidence_interval/interval_conclusion_view.cpp \
  controllers/confidence_interval/interval_curve_view.cpp \
  controllers/confidence_interval/interval_graph_controller.cpp \
  controllers/confidence_interval/interval_graph_view.cpp \
  controllers/store/input_store_controller.cpp \
  controllers/store/store_table_cell.cpp \
  controllers/tables/anova_data_source.cpp \
  controllers/tables/categorical_cell.cpp \
  controllers/tables/categorical_controller.cpp \
  controllers/tables/categorical_table_cell.cpp \
  controllers/tables/dynamic_cells_data_source.cpp \
  controllers/tables/homogeneity_data_source.cpp \
  controllers/tables/input_anova_table_cell.cpp \
  controllers/tables/input_goodness_table_cell.cpp \
  controllers/tables/input_homogeneity_table_cell.cpp \
  controllers/tables/results_goodness_table_cell.cpp \
  controllers/tables/results_homogeneity_table_cell.cpp \
  controllers/significance_test/hypothesis_controller.cpp \
  controllers/significance_test/hypothesis_editable_controller.cpp \
  controllers/significance_test/hypothesis_display_only_controller.cpp \
  controllers/significance_test/legend_view.cpp \
  controllers/significance_test/test_curve_view.cpp \
  controllers/significance_test/test_graph_controller.cpp \
  controllers/significance_test/test_graph_view.cpp \
  controllers/significance_test/zoom_hint_view.cpp \
  controllers/comparison_operator_popup_data_source.cpp \
  controllers/dataset_controller.cpp \
  controllers/input_anova_controller.cpp \
  controllers/input_controller.cpp \
  controllers/results_controller.cpp \
  controllers/test_controller.cpp \
  controllers/type_controller.cpp \
  inference_icon.png \
  images/confidence_interval.png \
  images/significance_test.png \
) \
  models/anova_test.cpp \
  models/chi2_test.cpp \
  models/confidence_interval.cpp \
  models/goodness_test.cpp \
  models/homogeneity_test.cpp \
  models/inference_model.cpp \
  models/one_mean_statistic.cpp \
  models/significance_test.cpp \
  models/slope_t_statistic.cpp \
  models/input_table.cpp \
  models/input_table_from_store.cpp \
  models/two_means_statistic.cpp \
  test/inference_models.cpp:+test \
)

$(call depends_on_image,$(PATH_apps)/inference/app.cpp,$(PATH_apps)/inference/inference_icon.png)

$(call depends_on_transparent_image,$(PATH_apps)/inference/app.cpp,$(addprefix $(PATH_apps)/inference/images/,\
  confidence_interval.png \
  significance_test.png \
))

$(call i18n_with_universal,inference/i18n/base)
