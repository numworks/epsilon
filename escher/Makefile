SFLAGS += -Iescher/include

objs += $(addprefix escher/src/,\
  app.o\
  buffer_text_view.o\
  button.o\
  container.o\
  header_view_controller.o\
  image_view.o\
  invocation.o\
  input_view_controller.o\
  list_view.o\
  list_view_data_source.o\
  metric.o\
  palette.o\
  pointer_text_view.o\
  responder.o\
  scroll_view.o\
  scroll_view_indicator.o\
  simple_list_view_data_source.o\
  simple_table_view_data_source.o\
  solid_color_view.o\
  stack_view.o\
  stack_view_controller.o\
  switch_list_view_cell.o\
  switch_view.o\
  tab_view.o\
  tab_view_cell.o\
  tab_view_controller.o\
  table_view.o\
  table_view_cell.o\
  list_view_cell.o\
  table_view_data_source.o\
  text_field.o\
  text_list_view_cell.o\
  text_view.o\
  tiled_view.o\
  view.o\
  view_controller.o\
  window.o\
)

INLINER := escher/image/inliner

$(INLINER): escher/image/inliner.c
	@echo "HOSTCC  $@"
	@$(HOSTCC) `libpng-config --cflags` `libpng-config --ldflags` $< -o $@

inline_image_headers := $(inline_images:.png=.h)
inline_image_sources := $(inline_images:.png=.cpp)
inline_image_objs := $(inline_images:.png=.o)

$(inline_image_sources) : %.cpp : %.png $(INLINER)
	@echo "INLINER $@"
	@$(INLINER) $<

objs += $(inline_image_objs)
products += $(INLINER) $(inline_image_headers) $(inline_image_sources)
