SFLAGS += -Iescher/include

objs += $(addprefix escher/src/,\
  alternate_empty_view_controller.o\
  app.o\
  buffer_text_view.o\
  button.o\
  chevron_view.o\
  container.o\
  editable_text_cell.o\
  even_odd_cell.o\
  even_odd_buffer_text_cell.o\
  even_odd_editable_text_cell.o\
  even_odd_expression_cell.o\
  even_odd_pointer_text_cell.o\
  expression_table_cell.o\
  expression_table_cell_with_pointer.o\
  expression_view.o\
  header_view_controller.o\
  header_view_delegate.o\
  highlight_cell.o\
  image_view.o\
  invocation.o\
  invocation_timer.o\
  input_view_controller.o\
  key_view.o\
  list_view_data_source.o\
  modal_view_controller.o\
  palette.o\
  pointer_table_cell.o\
  pointer_table_cell_with_buffer.o\
  pointer_table_cell_with_chevron.o\
  pointer_table_cell_with_chevron_and_pointer.o\
  pointer_table_cell_with_chevron_and_expression.o\
  pointer_table_cell_with_editable_text.o\
  pointer_table_cell_with_pointer.o\
  pointer_table_cell_with_switch.o\
  pointer_text_view.o\
  responder.o\
  run_loop.o\
  scroll_view.o\
  scroll_view_indicator.o\
  selectable_table_view.o\
  simple_list_view_data_source.o\
  simple_table_view_data_source.o\
  solid_color_view.o\
  stack_view.o\
  stack_view_controller.o\
  switch_view.o\
  tab_view.o\
  tab_view_cell.o\
  tab_view_controller.o\
  table_cell.o\
  table_view.o\
  table_view_data_source.o\
  text_field.o\
  text_view.o\
  tiled_view.o\
  toolbox.o\
  timer.o\
  view.o\
  view_controller.o\
  warning_controller.o\
  window.o\
)

INLINER := escher/image/inliner

$(INLINER): escher/image/inliner.c
	@echo "HOSTCC  $@"
	@$(HOSTCC) -std=c99 `libpng-config --cflags` `libpng-config --ldflags` $< -o $@

%.h %.cpp : %.png $(INLINER)
	@echo "INLINER $@"
	@$(INLINER) $<

INLINER_PRODUCTS = $(1:.png=.h) $(1:.png=.cpp) $(1:.png=.o)

products += $(INLINER)
