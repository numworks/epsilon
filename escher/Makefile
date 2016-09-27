SFLAGS += -Iescher/include

objs += $(addprefix escher/src/,\
  app.o\
  childless_view.o\
  container.o\
  invocation.o\
  metric.o\
  palette.o\
  responder.o\
  scroll_view.o\
  scroll_view_indicator.o\
  solid_color_view.o\
  stack_view.o\
  stack_view_controller.o\
  switch_table_view_cell.o\
  switch_view.o\
  tab_view.o\
  tab_view_cell.o\
  tab_view_controller.o\
  table_view.o\
  table_view_cell.o\
  text_field.o\
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
