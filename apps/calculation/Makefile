apps += Calculation::App
app_headers += apps/calculation/app.h

app_calculation_test_src += $(addprefix apps/calculation/,\
  calculation.cpp \
  calculation_store.cpp \
  additional_outputs/unit_comparison_helper.cpp \
  additional_outputs/scientific_notation_helper.cpp \
)

app_calculation_src = $(addprefix apps/calculation/,\
  additional_outputs/chained_expressions_list_controller.cpp \
  additional_outputs/complex_graph_cell.cpp \
  additional_outputs/complex_model.cpp \
  additional_outputs/complex_list_controller.cpp \
  additional_outputs/expression_with_equal_sign_view.cpp \
  additional_outputs/expressions_list_controller.cpp \
  additional_outputs/function_graph_cell.cpp \
  additional_outputs/function_list_controller.cpp \
  additional_outputs/function_model.cpp \
  additional_outputs/illustrated_expressions_list_controller.cpp \
  additional_outputs/illustration_cell.cpp \
  additional_outputs/integer_list_controller.cpp \
  additional_outputs/scrollable_three_expressions_view.cpp \
  additional_outputs/list_controller.cpp \
  additional_outputs/matrix_list_controller.cpp \
  additional_outputs/rational_list_controller.cpp \
  additional_outputs/scientific_notation_list_controller.cpp \
  additional_outputs/trigonometry_graph_cell.cpp \
  additional_outputs/trigonometry_list_controller.cpp \
  additional_outputs/trigonometry_model.cpp \
  additional_outputs/unit_list_controller.cpp \
  additional_outputs/vector_graph_cell.cpp \
  additional_outputs/vector_list_controller.cpp \
  additional_outputs/vector_model.cpp \
  app.cpp \
  calculation_selectable_table_view.cpp \
  edit_expression_controller.cpp \
  expression_field.cpp \
  history_view_cell.cpp \
  history_controller.cpp \
)

app_calculation_src += $(app_calculation_test_src)
apps_src += $(app_calculation_src)

i18n_files += $(call i18n_without_universal_for,calculation/base)
i18n_files += $(call i18n_without_universal_for,calculation/additional_outputs/unit_comparison)

tests_src += $(addprefix apps/calculation/test/,\
  calculation_store.cpp\
)

$(eval $(call depends_on_image,apps/calculation/app.cpp,apps/calculation/calculation_icon.png))
