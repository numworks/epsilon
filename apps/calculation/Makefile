SOURCES_apps += $(addprefix $(PATH_apps)/calculation/, \
  additional_results/additional_results_type.cpp \
  additional_results/unit_comparison_helper.cpp \
  additional_results/scientific_notation_helper.cpp \
  additional_results/vector_helper.cpp \
  calculation.cpp \
  calculation_store.cpp \
$(addsuffix :-test, \
  additional_results/additional_results_controller.cpp \
  additional_results/chained_expressions_list_controller.cpp \
  additional_results/complex_graph_cell.cpp \
  additional_results/complex_list_controller.cpp \
  additional_results/complex_model.cpp \
  additional_results/expressions_list_controller.cpp \
  additional_results/function_graph_cell.cpp \
  additional_results/function_list_controller.cpp \
  additional_results/function_model.cpp \
  additional_results/illustrated_expressions_list_controller.cpp \
  additional_results/illustration_cell.cpp \
  additional_results/integer_list_controller.cpp \
  additional_results/layout_with_equal_sign_view.cpp \
  additional_results/matrix_list_controller.cpp \
  additional_results/rational_list_controller.cpp \
  additional_results/scientific_notation_list_controller.cpp \
  additional_results/scrollable_three_layouts_view.cpp \
  additional_results/trigonometry_graph_cell.cpp \
  additional_results/trigonometry_list_controller.cpp \
  additional_results/trigonometry_model.cpp \
  additional_results/unit_list_controller.cpp \
  additional_results/vector_graph_cell.cpp \
  additional_results/vector_list_controller.cpp \
  additional_results/vector_model.cpp \
  app.cpp \
  calculation_selectable_list_view.cpp \
  edit_expression_controller.cpp \
  history_controller.cpp \
  history_view_cell.cpp \
  layout_field.cpp \
  calculation_icon.png \
) \
  test/calculation_store.cpp:+test \
)

$(eval $(call depends_on_image,$(PATH_apps)/calculation/app.cpp,$(PATH_apps)/calculation/calculation_icon.png))

$(call i18n_without_universal,calculation/i18n/base)
$(call i18n_without_universal,calculation/additional_results/i18n/unit_comparison)
