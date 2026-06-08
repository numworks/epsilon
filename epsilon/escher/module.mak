$(call create_module,escher,1,$(addprefix src/, \
  abstract_text_field.cpp \
  alternate_empty_nested_menu_controller.cpp \
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
  chained_text_field_delegate.cpp \
  chevron_view.cpp \
  clipboard.cpp \
  container.cpp \
  color_picker.cpp \
  dropdown_view.cpp \
  editable_expression_cell.cpp \
  editable_expression_model_cell.cpp \
  editable_text_cell.cpp \
  ellipsis_view.cpp \
  even_odd_cell.cpp \
  even_odd_cell_with_ellipsis.cpp \
  even_odd_buffer_text_cell.cpp \
  even_odd_editable_text_cell.cpp \
  even_odd_expression_cell.cpp \
  even_odd_message_text_cell.cpp \
  explicit_list_view_data_source.cpp \
  expression_input_bar.cpp \
  layout_view.cpp \
  layout_style.cpp \
  gauge_view.cpp \
  glyphs_view.cpp \
  heavy_table_size_manager.cpp \
  highlight_cell.cpp \
  highlight_image_cell.cpp \
  horizontal_or_vertical_layout.cpp \
  themed_image_view.cpp \
  init.cpp \
  editable_field.cpp \
  invocation.cpp \
  input_view_controller.cpp \
  key_view.cpp \
  layout_field.cpp \
  layout_preferences.cpp \
  list_view_data_source.cpp \
  menu_cell.cpp \
  menu_cell_with_editable_text.cpp \
  message_text_view.cpp \
  message_view.cpp \
  modal_view_controller.cpp \
  modal_view_empty_controller.cpp \
  nested_menu_controller.cpp \
  pointer_text_view.cpp \
  pop_up_controller.cpp \
  preface_data_source.cpp \
  prefaced_table_view.cpp \
  prefaced_twice_table_view.cpp \
  responder.cpp \
  run_loop.cpp \
  scroll_view.cpp \
  scroll_view_data_source.cpp \
  scroll_view_indicator.cpp \
  scrollable_layout_view.cpp \
  scrollable_view.cpp \
  selectable_list_view.cpp \
  selectable_list_view_controller.cpp \
  selectable_list_view_delegate.cpp \
  list_with_top_and_bottom_controller.cpp \
  scrollable_multiple_layouts_view.cpp \
  scrollable_two_layouts_cell.cpp \
  selectable_table_view.cpp \
  selectable_table_view_delegate.cpp \
  solid_color_view.cpp \
  stack_view.cpp \
  stack_view_controller.cpp \
  stack_header_view.cpp \
  subapp_cell.cpp \
  switch_view.cpp \
  tab_union_view_controller.cpp \
  tab_view.cpp \
  tab_view_cell.cpp \
  tab_view_controller.cpp \
  tab_view_data_source.cpp \
  table_grid_view.cpp \
  table_view_with_frozen_header.cpp \
  table_size_1D_manager.cpp \
  table_view.cpp \
  table_view_data_source.cpp \
  text_cursor_view.cpp \
  text_area.cpp \
  text_format.cpp \
  text_input.cpp \
  text_input_helpers.cpp \
  text_view.cpp \
  timer.cpp \
  toggleable_dot_view.cpp \
  toggleable_view.cpp \
  toolbox.cpp \
  transparent_themed_image_view.cpp \
  unequal_view.cpp \
  view.cpp \
  view_controller.cpp \
  warning_controller.cpp \
  window.cpp \
) \
  test/layout_field.cpp:+test \
  test/text_field.cpp:+test \
)

ESCHER_VIEW_LOGGING ?= 0

ifneq ($(ESCHER_VIEW_LOGGING),0)
SFLAGS_escher += -DESCHER_VIEW_LOGGING=1
endif

$(call assert_defined,KANDINSKY_fonts_dependencies)

$(call all_objects_for,$(SOURCES_escher)): $(KANDINSKY_fonts_dependencies)

# Inliner

_escher_inliner := $(TOOLS_DIRECTORY)/inliner.bin

$(call import_module,escher_inliner,$(PATH_escher)/image)
$(call create_module,escher_inliner,1,inliner.c)

$(call create_tool,inliner, \
  omg.lz4only \
  escher_inliner \
)

$(_escher_inliner): TOOLS_CFLAGS += -std=c11 $(shell pkg-config libpng --cflags)
$(_escher_inliner): TOOLS_LDFLAGS += $(shell pkg-config libpng --libs)

$(OUTPUT_DIRECTORY)/%.cpp: %.png $(_escher_inliner) | $$(@D)/.
	$(call rule_label,INLINE)
	$(_escher_inliner) $< $(basename $@).h $(basename $@).cpp $(INLINER_ARGS)

# Helpers for sources using images

# depends_on_themed_image, <module>, <list of cpp>, <light png>, <dark png>, [tastes]
# Inlines both PNGs directly into a single ThemedImage .h/.cpp pair via the
# inliner's --themed mode, and appends the generated .cpp to SOURCES_<module>.
# The optional <tastes> argument is appended to the generated source entry
# (e.g. ":-test" to exclude from the test binary). Defaults to empty (image
# linked into every binary). Pass ":-test" when the image is only referenced
# from code that is itself excluded from the test binary, to keep the test
# binary smaller. The output stem is deduced from the PNG paths:
# <dir>/light/<base>_light_<rest>.png and <dir>/dark/<base>_dark_<rest>.png
# both yield <dir>/<base>_<rest>, so the generated header is included as
# <base>_<rest>.h. Both PNGs must yield the same stem.

# _themed_image_stem, <light or dark png>, <light|dark>
# Strips the light/dark directory segment and the _light/_dark name infix.
_themed_image_stem = $(patsubst %.png,%,$(subst $2/,,$(subst _$2,,$1)))

define _depends_on_themed_image_body
$(call all_objects_for,$(strip $2)): $(OUTPUT_DIRECTORY)/$3.cpp
$(call all_objects_for,$(strip $2)): SFLAGS += -I$(OUTPUT_DIRECTORY)/$(dir $3)
$(OUTPUT_DIRECTORY)/$3.h: $(OUTPUT_DIRECTORY)/$3.cpp ;
$(OUTPUT_DIRECTORY)/$3.cpp: $(strip $4) $(strip $5) $(_escher_inliner) | $(OUTPUT_DIRECTORY)/$(dir $3).
	$$(call rule_label,INLINE)
	$(_escher_inliner) --themed $(strip $4) $(strip $5) $(OUTPUT_DIRECTORY)/$3.h $(OUTPUT_DIRECTORY)/$3.cpp $$(INLINER_ARGS)
SOURCES_$(strip $1) += $$(call generated_sources_for,$3.cpp$6)
endef

# Deduces the output stem from the light/dark PNG paths and checks they agree.
define depends_on_themed_image
$(if $(filter-out $(call _themed_image_stem,$(strip $3),light),$(call _themed_image_stem,$(strip $4),dark)),$(error depends_on_themed_image: light "$(strip $3)" and dark "$(strip $4)" yield different stems))
$(eval $(call _depends_on_themed_image_body,$1,$2,$(call _themed_image_stem,$(strip $3),light),$3,$4,$5))
endef

# depends_on_transparent_themed_image, <module>, <list of cpp>, <light png>, <dark png>, [tastes]
# Like depends_on_themed_image but passes --transparent to the inliner.
define depends_on_transparent_themed_image
$(eval \
$(OUTPUT_DIRECTORY)/$(call _themed_image_stem,$(strip $3),light).cpp: INLINER_ARGS += --transparent
)
$(call depends_on_themed_image,$1,$2,$3,$4,$5)
endef

# Body of depends_on_selectable_themed_image: like _depends_on_themed_image_body
# but feeds an extra selected_dark PNG to the inliner via --selectable.
define _depends_on_selectable_themed_image_body
$(call all_objects_for,$(strip $2)): $(OUTPUT_DIRECTORY)/$3.cpp
$(call all_objects_for,$(strip $2)): SFLAGS += -I$(OUTPUT_DIRECTORY)/$(dir $3)
$(OUTPUT_DIRECTORY)/$3.h: $(OUTPUT_DIRECTORY)/$3.cpp ;
$(OUTPUT_DIRECTORY)/$3.cpp: $(strip $4) $(strip $5) $(strip $6) $(_escher_inliner) | $(OUTPUT_DIRECTORY)/$(dir $3).
	$$(call rule_label,INLINE)
	$(_escher_inliner) --themed --selectable $(strip $4) $(strip $5) $(strip $6) $(OUTPUT_DIRECTORY)/$3.h $(OUTPUT_DIRECTORY)/$3.cpp $$(INLINER_ARGS)
SOURCES_$(strip $1) += $$(call generated_sources_for,$3.cpp$7)
endef

# depends_on_selectable_themed_image, <module>, <list of cpp>, <light png>, <dark png>, <selectable dark png>, [tastes]
# Like depends_on_themed_image but takes a third PNG for the selected variant
# of the dark theme. The generated ThemedImage embeds it as selectableDark.
define depends_on_selectable_themed_image
$(if $(filter-out $(call _themed_image_stem,$(strip $3),light),$(call _themed_image_stem,$(strip $4),dark)),$(error depends_on_selectable_themed_image: light "$(strip $3)" and dark "$(strip $4)" yield different stems))
$(if $(filter-out $(call _themed_image_stem,$(strip $3),light),$(call _themed_image_stem,$(strip $5),selected_dark)),$(error depends_on_selectable_themed_image: light "$(strip $3)" and selected_dark "$(strip $5)" yield different stems))
$(eval $(call _depends_on_selectable_themed_image_body,$1,$2,$(call _themed_image_stem,$(strip $3),light),$3,$4,$5,$6))
endef

# depends_on_transparent_selectable_themed_image, <module>, <list of cpp>, <light png>, <dark png>, <selected dark png>, [tastes]
# Like depends_on_selectable_themed_image but passes --transparent to the inliner.
define depends_on_transparent_selectable_themed_image
$(eval \
$(OUTPUT_DIRECTORY)/$(call _themed_image_stem,$(strip $3),light).cpp: INLINER_ARGS += --transparent
)
$(call depends_on_selectable_themed_image,$1,$2,$3,$4,$5,$6)
endef

$(call depends_on_transparent_selectable_themed_image,escher,$(PATH_escher)/src/dropdown_view.cpp,$(PATH_escher)/image/light/caret_light_icon.png,$(PATH_escher)/image/dark/caret_dark_icon.png,$(PATH_escher)/image/selected_dark/caret_selected_dark_icon.png)
