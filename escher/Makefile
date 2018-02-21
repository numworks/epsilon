SFLAGS += -Iescher/include

objs += $(addprefix escher/src/,\
  alternate_empty_view_controller.o\
  app.o\
  buffer_text_view.o\
  button.o\
  button_row_controller.o\
  chevron_view.o\
  clipboard.o\
  container.o\
  dynamic_view_controller.o\
  editable_text_cell.o\
  ellipsis_view.o\
  even_odd_cell.o\
  even_odd_cell_with_ellipsis.o\
  even_odd_buffer_text_cell.o\
  even_odd_editable_text_cell.o\
  even_odd_expression_cell.o\
  even_odd_message_text_cell.o\
  expression_table_cell.o\
  expression_table_cell_with_pointer.o\
  expression_view.o\
  highlight_cell.o\
  gauge_view.o\
  image_view.o\
  invocation.o\
  input_view_controller.o\
  key_view.o\
  list_view_data_source.o\
  message_table_cell.o\
  message_table_cell_with_buffer.o\
  message_table_cell_with_chevron.o\
  message_table_cell_with_chevron_and_message.o\
  message_table_cell_with_chevron_and_expression.o\
  message_table_cell_with_editable_text.o\
  message_table_cell_with_gauge.o\
  message_table_cell_with_message.o\
  message_table_cell_with_switch.o\
  message_text_view.o\
  message_tree.o\
  modal_view_controller.o\
  palette.o\
  pointer_text_view.o\
  responder.o\
  run_loop.o\
  scroll_view.o\
  scroll_view_data_source.o\
  scroll_view_indicator.o\
  scrollable_view.o\
  selectable_table_view.o\
  selectable_table_view_data_source.o\
  selectable_table_view_delegate.o\
  simple_list_view_data_source.o\
  simple_table_view_data_source.o\
  solid_color_view.o\
  stack_view.o\
  stack_view_controller.o\
  switch_view.o\
  tab_view.o\
  tab_view_cell.o\
  tab_view_controller.o\
  tab_view_data_source.o\
  table_cell.o\
  table_view.o\
  table_view_data_source.o\
  text_cursor_view.o\
  text_area.o\
  text_field.o\
  text_input.o\
  text_input_helpers.o\
  text_view.o\
  tiled_view.o\
  timer.o\
  toolbox.o\
  toolbox_message_tree.o\
  view.o\
  view_controller.o\
  warning_controller.o\
  window.o\
)

INLINER := escher/image/inliner

$(INLINER): escher/image/inliner.c
	@echo "HOSTCC  $@"
	$(Q) $(HOSTCC) -std=c99 `libpng-config --cflags` $< `libpng-config --ldflags` -o $@

%.h %.cpp : %.png $(INLINER)
	@echo "INLINER $@"
	$(Q) $(INLINER) $<

INLINER_PRODUCTS = $(1:.png=.h) $(1:.png=.cpp) $(1:.png=.o)

products += $(INLINER)
