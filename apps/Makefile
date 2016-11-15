include apps/home/Makefile
include apps/graph/Makefile
include apps/probability/Makefile
include apps/calculation/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  constant.o\
  expression_text_field_delegate.o\
  main.o\
  node.o\
  node_list_view_controller.o\
  node_navigation_controller.o\
  toolbox_controller.o\
  toolbox_leaf_cell.o\
  toolbox_node.o\
  variable_box_controller.o\
  variable_box_leaf_cell.o\
  variable_box_node.o\
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
