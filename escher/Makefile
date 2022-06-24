SFLAGS += -Iescher/include

ifeq ($(THEME_REPO),local)
$(eval $(call rule_for, \
  THEME, \
  escher/palette.h, \
  $(addprefix themes/themes/local/, $(addsuffix .json, $(THEME_NAME))), \
  $$(PYTHON) themes/themes_manager.py $(THEME_REPO) $(THEME_NAME) $$@ $(BUILD_DIR)/, \
  global \
))
else
$(eval $(call rule_for, \
  THEME, \
  escher/palette.h, \
  themes/themes/git/.empty-folder, \
  $$(PYTHON) themes/themes_manager.py $(THEME_REPO) $(THEME_NAME) $$@ $(BUILD_DIR)/, \
  global \
))
endif


escher_src += $(addprefix escher/src/,\
  alternate_empty_view_controller.cpp \
  animation_timer.cpp \
  app.cpp \
  background_view.cpp \
  bank_view_controller.cpp \
  bordered.cpp \
  buffer_text_view.cpp \
  button.cpp \
  button_state.cpp \
  button_row_controller.cpp \
  chevron_view.cpp \
  clipboard.cpp \
  container.cpp \
  editable_text_cell.cpp \
  ellipsis_view.cpp \
  expression_field.cpp \
  even_odd_cell.cpp \
  even_odd_cell_with_ellipsis.cpp \
  even_odd_buffer_text_cell.cpp \
  even_odd_editable_text_cell.cpp \
  even_odd_expression_cell.cpp \
  even_odd_message_text_cell.cpp \
  expression_table_cell.cpp \
  expression_table_cell_with_pointer.cpp \
  expression_table_cell_with_expression.cpp \
  expression_view.cpp \
  highlight_cell.cpp \
  gauge_view.cpp \
  icon_view.cpp \
  image_view.cpp \
  input_event_handler.cpp \
  invocation.cpp \
  input_view_controller.cpp \
  key_view.cpp \
  layout_field.cpp \
  list_view_data_source.cpp \
  message_table_cell.cpp \
  message_table_cell_with_buffer.cpp \
  message_table_cell_with_chevron.cpp \
  message_table_cell_with_chevron_and_buffer.cpp \
  message_table_cell_with_chevron_and_message.cpp \
  message_table_cell_with_chevron_and_expression.cpp \
  message_table_cell_with_editable_text.cpp \
  message_table_cell_with_expression.cpp \
  message_table_cell_with_gauge.cpp \
  message_table_cell_with_message.cpp \
  message_table_cell_with_switch.cpp \
  message_text_view.cpp \
  metric.cpp \
  modal_view_controller.cpp \
  modal_view_empty_controller.cpp \
  nested_menu_controller.cpp \
  palette.cpp \
  pointer_text_view.cpp \
  pop_up_controller.cpp \
  responder.cpp \
  run_loop.cpp \
  scroll_view.cpp \
  scroll_view_data_source.cpp \
  scroll_view_indicator.cpp \
  scrollable_expression_view.cpp \
  scrollable_view.cpp \
  selectable_table_view.cpp \
  simple_list_view_data_source.cpp \
  simple_table_view_data_source.cpp \
  slideable_message_text_view.cpp \
  solid_color_view.cpp \
  stack_view.cpp \
  stack_view_controller.cpp \
  switch_view.cpp \
  tab_view.cpp \
  tab_view_cell.cpp \
  tab_view_controller.cpp \
  tab_view_data_source.cpp \
  table_cell.cpp \
  table_view.cpp \
  table_view_data_source.cpp \
  text_cursor_view.cpp \
  text_area.cpp \
  text_field.cpp \
  text_input.cpp \
  text_input_helpers.cpp \
  text_view.cpp \
  tiled_view.cpp \
  timer.cpp \
  toggleable_dot_view.cpp \
  toggleable_view.cpp \
  toolbox.cpp \
  transparent_view.cpp \
  view.cpp \
  view_controller.cpp \
  warning_controller.cpp \
  window.cpp \
)

tests_src += $(addprefix escher/test/,\
  clipboard.cpp \
  layout_field.cpp\
)

$(eval $(call rule_for, \
  HOSTCC, \
  escher/image/inliner, \
  escher/image/inliner.c $(addprefix ion/src/external/lz4/, lz4.c lz4hc.c), \
  $$(HOSTCC) -std=c99 `libpng-config --cflags` $$^ `libpng-config --ldflags` -o $$@, \
  global \
))

INLINER := $(BUILD_DIR)/escher/image/inliner

.PRECIOUS: $(BUILD_DIR)/%.h $(BUILD_DIR)/%.cpp
$(eval $(call rule_for, \
  INLINER, \
  %.h %.cpp, \
  $(BUILD_DIR)/%.png $$(INLINER), \
  $$(INLINER) $$< $$(basename $$@).h $$(basename $$@).cpp, \
  global \
))

# Mark a .cpp file as depending on a .png one
# This is called with $1 = code.cpp and $2 = image.png
# First, we mark code.o as requiring image.o. Rules will take care of inlining
# the PNG and building the inlined cpp file. Second, we add the directory
# corresponding to the one of code.cpp in the output dir as a header search
# path. Since $1 can be a list, we have to map with foreach.
define depends_on_image
$(call object_for,$(1)): $(call object_for,$(2))
$(call object_for,$(1)): SFLAGS += $(foreach d,$(sort $(dir $(call object_for,$(1)))),-I$(d))
escher_src += $(2)
image_list += $(2)
endef
