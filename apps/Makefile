include apps/calculation/Makefile
include apps/graph/Makefile
include apps/home/Makefile
include apps/hardware_test/Makefile
include apps/on_boarding/Makefile
include apps/probability/Makefile
include apps/regression/Makefile
include apps/sequence/Makefile
include apps/settings/Makefile
include apps/shared/Makefile
include apps/statistics/Makefile
include apps/code/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  apps_window.o\
  battery_timer.o\
  battery_view.o\
  constant.o\
  backlight_dimming_timer.o\
  empty_battery_window.o\
  exam_pop_up_controller.o\
  global_preferences.o\
  i18n.o\
  led_timer.o\
  lock_view.o\
  main.o\
  math_toolbox.o\
  node.o\
  shift_alpha_lock_view.o\
  suspend_timer.o\
  title_bar_view.o\
  toolbox_node.o\
  usb_timer.o\
  variable_box_controller.o\
  variable_box_leaf_cell.o\
)

app_images += apps/exam_icon.png

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
