include apps/shared/Makefile
include apps/home/Makefile
include apps/on_boarding/Makefile
include apps/hardware_test/Makefile
snapshots =

# All selected apps are included. Each Makefile below is responsible for setting
# the $snapshots variable (name of the snapshot class) and the $snapshot_headers
# (path to the snapshot header).
$(foreach i,${EPSILON_APPS},$(eval include apps/$(i)/Makefile))

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
  shift_alpha_lock_view.o\
  suspend_timer.o\
  title_bar_view.o\
  usb_timer.o\
  variable_box_controller.o\
  variable_box_leaf_cell.o\
)

snapshots_declaration = $(foreach i,$(snapshots),$(i) m_snapshot$(subst :,,$(i));)
snapshots_construction = $(foreach i,$(snapshots),,m_snapshot$(subst :,,$(i))())
snapshots_list = $(foreach i,$(snapshots),,&m_snapshot$(subst :,,$(i)))
snapshots_count = $(words $(snapshots))
snapshot_includes = $(foreach i,$(snapshot_headers),-include $(i) )
epsilon_app_names = '$(foreach i,${EPSILON_APPS},"$(i)", )'

apps/apps_container_storage.o apps/main.o: CXXFLAGS += $(snapshot_includes) -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(snapshots_construction)" -DAPPS_CONTAINER_SNAPSHOT_LIST="$(snapshots_list)" -DAPPS_CONTAINER_SNAPSHOT_COUNT=$(snapshots_count) -DEPSILON_APPS_NAMES=$(epsilon_app_names)


i18n_files += $(addprefix apps/language_,$(addsuffix .universal.i18n, $(EPSILON_I18N)))
i18n_files += $(addprefix apps/,\
  shared.de.i18n\
  shared.en.i18n\
  shared.es.i18n\
  shared.fr.i18n\
  shared.pt.i18n\
  shared.universal.i18n\
  toolbox.de.i18n\
  toolbox.en.i18n\
  toolbox.es.i18n\
  toolbox.fr.i18n\
  toolbox.pt.i18n\
  variables.de.i18n\
  variables.en.i18n\
  variables.es.i18n\
  variables.fr.i18n\
  variables.pt.i18n\
)

apps/i18n.h: apps/i18n.cpp
apps/i18n.cpp: $(i18n_files)
	@echo "I18N    $@"
	$(Q) $(PYTHON) apps/i18n.py --header $(subst .cpp,.h,$@) --implementation $@ --locales $(EPSILON_I18N) --files $^

$(app_objs): apps/i18n.h

products += apps/i18n.h apps/i18n.cpp

app_images += apps/exam_icon.png

# Tracking which source file uses which image is painful. But we need to ensure
# that a .png file has been inlined before building any source file that uses
# said image (because it will expect the ".h" file to be there).
# As a shortcut, we simply say that every app file depends on every image. In
# practice, this forces all the images to be before the app.

app_image_objs := $(app_images:.png=.o)
.SECONDARY: $(app_images:.png=.cpp)
$(app_objs): $(app_image_objs)

epsilon.$(EXE): $(app_objs) $(app_image_objs)

products += epsilon.$(EXE) $(app_objs) $(call INLINER_PRODUCTS,$(app_images))
