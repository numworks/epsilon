SFLAGS += -Iescher/include

escher_src += $(addprefix escher/src/,\
  abstract_button_cell.cpp \
  abstract_text_field.cpp \
  alternate_empty_view_controller.cpp \
  alternate_view_controller.cpp \
  app.cpp \
  bank_view_controller.cpp \
  blink_timer.cpp \
  bordered.cpp \
  bordering_view.cpp \
  buffer_text_view.cpp \
  button_cell.cpp \
  button_state.cpp \
  button_row_controller.cpp \
  buffer_table_cell.cpp \
  buffer_table_cell_with_message.cpp \
  buffer_table_cell_with_editable_text.cpp \
  cell_with_margins.cpp \
  chained_text_field_delegate.cpp \
  chevron_view.cpp \
  clipboard.cpp \
  container.cpp \
  dropdown_view.cpp \
  editable_expression_cell.cpp \
  editable_expression_model_cell.cpp \
  editable_text_cell.cpp \
  ellipsis_view.cpp \
  expression_field.cpp \
  even_odd_cell.cpp \
  even_odd_cell_with_ellipsis.cpp \
  even_odd_buffer_text_cell.cpp \
  even_odd_editable_text_cell.cpp \
  even_odd_expression_cell.cpp \
  even_odd_message_text_cell.cpp \
  expression_cell_with_editable_text_with_message.cpp \
  expression_input_bar.cpp \
  expression_table_cell.cpp \
  expression_table_cell_with_buffer.cpp \
  expression_table_cell_with_expression.cpp \
  expression_table_cell_with_message.cpp \
  expression_view.cpp \
  highlight_cell.cpp \
  highlight_image_cell.cpp \
  horizontal_or_vertical_layout.cpp \
  gauge_view.cpp \
  image_view.cpp \
  init.cpp \
  input_event_handler.cpp \
  invocation.cpp \
  input_view_controller.cpp \
  key_view.cpp \
  layout_field.cpp \
  list_view_data_source.cpp \
  message_table_cell.cpp \
  message_table_cell_with_buffer.cpp \
  message_table_cell_with_chevron_and_buffer.cpp \
  message_table_cell_with_chevron_and_message.cpp \
  message_table_cell_with_chevron_and_expression.cpp \
  message_table_cell_with_editable_text.cpp \
  message_table_cell_with_editable_text_with_message.cpp \
  message_table_cell_with_expression.cpp \
  message_table_cell_with_gauge.cpp \
  message_table_cell_with_message.cpp \
  message_table_cell_with_message_with_buffer.cpp \
  message_text_view.cpp \
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
  selectable_list_view_controller.cpp \
  selectable_table_view.cpp \
  solid_color_view.cpp \
  stack_view.cpp \
  stack_view_controller.cpp \
  stack_header_view.cpp \
  subapp_cell.cpp \
  switch_view.cpp \
  tab_view.cpp \
  tab_view_cell.cpp \
  tab_view_controller.cpp \
  tab_view_data_source.cpp \
  table_view_with_frozen_header.cpp \
  table_view_with_top_and_bottom_views.cpp \
  table_cell.cpp \
  table_size_1D_manager.cpp \
  table_view.cpp \
  table_view_data_source.cpp \
  text_cursor_view.cpp \
  text_area.cpp \
  text_input.cpp \
  text_input_helpers.cpp \
  text_view.cpp \
  timer.cpp \
  toggleable_dot_view.cpp \
  toggleable_view.cpp \
  toolbox.cpp \
  transparent_image_view.cpp \
  transparent_image_cell_with_message.cpp \
  transparent_view.cpp \
  unequal_view.cpp \
  view.cpp \
  view_controller.cpp \
  warning_controller.cpp \
  window.cpp \
)

tests_src += $(addprefix escher/test/,\
  clipboard.cpp \
  layout_field.cpp \
)

$(eval $(call rule_for, \
  HOSTCC, \
  escher/image/inliner, \
  escher/image/inliner.c $(addprefix ion/src/external/lz4/, lz4.c lz4hc.c), \
  $$(HOSTCC) -std=c11 `libpng-config --cflags` $$^ `libpng-config --ldflags` -o $$@, \
  global \
))

INLINER := $(BUILD_DIR)/escher/image/inliner
INLINER_ARGS ?=

.PRECIOUS: $(BUILD_DIR)/%.h $(BUILD_DIR)/%.cpp
$(eval $(call rule_for, \
  INLINER, \
  %.cpp, \
  %.png $$(INLINER), \
  $$(INLINER) $$< $$(basename $$@).h $$(basename $$@).cpp $$(INLINER_ARGS), \
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
endef

define depends_on_transparent_image
# Set INLINE_ARGS for Header and CPP files
$(call object_for,$(2)): INLINER_ARGS += --transparent
$(call depends_on_image,$(1),$(2))
endef

$(eval $(call depends_on_transparent_image,escher/src/dropdown_view.cpp,$(addprefix escher/image/,\
  caret.png \
)))

ifeq ($(DEBUG),1)
  ifeq ($(PLATFORM),simulator)
    ESCHER_VIEW_LOGGING ?= 1
  endif
endif

ifdef ESCHER_VIEW_LOGGING
SFLAGS += -DESCHER_VIEW_LOGGING=$(ESCHER_VIEW_LOGGING)
endif
