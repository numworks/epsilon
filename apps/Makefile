include apps/calculation/Makefile
include apps/graph/Makefile
include apps/home/Makefile
include apps/probability/Makefile
include apps/regression/Makefile
include apps/settings/Makefile
include apps/statistics/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  apps_window.o\
  banner_view.o\
  battery_view.o\
  constant.o\
  cursor_view.o\
  curve_view.o\
  curve_view_cursor.o\
  curve_view_range.o\
  editable_cell_table_view_controller.o\
  expression_text_field_delegate.o\
  float_pair_store.o\
  float_parameter_controller.o\
  interactive_curve_view_controller.o\
  interactive_curve_view_range.o\
  main.o\
  memoized_curve_view_range.o\
  node.o\
  node_list_view_controller.o\
  node_navigation_controller.o\
  preference.o\
  range_parameter_controller.o\
  store_controller.o\
  store_parameter_controller.o\
  text_field_delegate_app.o\
  title_bar_view.o\
  toolbox_controller.o\
  toolbox_leaf_cell.o\
  toolbox_node.o\
  variable_box_controller.o\
  variable_box_leaf_cell.o\
  zoom_parameter_controller.o\
)

# Tracking which source file uses which image is painful. But we need to ensure
# that a .png file has been inlined before building any source file that uses
# said image (because it will expect the ".h" file to be there).
# As a shortcut, we simply say that every app file depends on every image. In
# practice, this forces all the images to be before the app.

app_image_objs := $(app_images:.png=.o)
.SECONDARY: $(app_images:.png=.cpp)
$(app_objs): $(app_image_objs)

app.$(EXE): $(app_objs) $(app_image_objs)

products += app.$(EXE) $(app_objs) $(call INLINER_PRODUCTS,$(app_images))
