include apps/calculation/Makefile
include apps/graph/Makefile
include apps/home/Makefile
include apps/probability/Makefile
include apps/regression/Makefile
include apps/settings/Makefile
include apps/shared/Makefile
include apps/statistics/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  apps_window.o\
  battery_view.o\
  constant.o\
  main.o\
  node.o\
  node_list_view_controller.o\
  node_navigation_controller.o\
  preferences.o\
  title_bar_view.o\
  toolbox_controller.o\
  toolbox_leaf_cell.o\
  toolbox_node.o\
  variable_box_controller.o\
  variable_box_leaf_cell.o\
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
