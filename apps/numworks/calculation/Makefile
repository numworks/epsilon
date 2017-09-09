ifneq (,$(findstring numworks.calculation,${APPS_LIST}))

app_objs += $(addprefix apps/numworks/calculation/,\
  app.o\
  calculation.o\
  calculation_store.o\
  edit_expression_controller.o\
  history_view_cell.o\
  history_controller.o\
  local_context.o\
  scrollable_expression_view.o\
  selectable_table_view.o\
  text_field.o\
)

tests += $(addprefix apps/numworks/calculation/test/,\
  calculation_store.cpp\
)
test_objs += $(addprefix apps/numworks/calculation/,\
  calculation.o\
  calculation_store.o\
)

app_images += apps/numworks/calculation/calculation_icon.png

endif
