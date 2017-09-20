include apps/shared/Makefile
include apps/home/Makefile
include apps/on_boarding/Makefile
include apps/hardware_test/Makefile
snapshots =

# You can select below which apps you want to include in your build, as well as
# their order on the home screen. Each Makefile below is responsible for setting
# the $snapshots variable (name of the snapshot class) and the $snapshot_headers
# (path to the snapshot header).
include apps/calculation/Makefile
include apps/graph/Makefile
include apps/sequence/Makefile
include apps/settings/Makefile
include apps/statistics/Makefile
include apps/probability/Makefile
include apps/regression/Makefile
include apps/code/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  apps_container_storage.o\
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

snapshots_declaration = $(foreach i,$(snapshots),$(i) m_snapshot$(subst :,,$(i));)
snapshots_construction = $(foreach i,$(snapshots),,m_snapshot$(subst :,,$(i))())
snapshots_list = $(foreach i,$(snapshots),,&m_snapshot$(subst :,,$(i)))
snapshots_count = $(words $(snapshots))
snapshot_includes = $(foreach i,$(snapshot_headers),-include $(i) )

apps/apps_container_storage.o apps/main.o: CXXFLAGS += $(snapshot_includes) -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(snapshots_construction)" -DAPPS_CONTAINER_SNAPSHOT_LIST="$(snapshots_list)" -DAPPS_CONTAINER_SNAPSHOT_COUNT=$(snapshots_count)

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
