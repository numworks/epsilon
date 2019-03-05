include apps/shared/Makefile
include apps/home/Makefile
include apps/on_boarding/Makefile
include apps/hardware_test/Makefile
include apps/usb/Makefile
apps =

# All selected apps are included. Each Makefile below is responsible for setting
# the $apps variable (name of the app class) and the $app_headers
# (path to the apps header).
$(foreach i,${EPSILON_APPS},$(eval include apps/$(i)/Makefile))

app_src += $(addprefix apps/,\
  apps_container.cpp \
  apps_container_storage.cpp \
  apps_window.cpp \
  backlight_dimming_timer.cpp \
  battery_timer.cpp \
  battery_view.cpp \
  constant.cpp \
  empty_battery_window.cpp \
  exam_pop_up_controller.cpp \
  global_preferences.cpp \
  i18n.cpp \
  lock_view.cpp \
  main.cpp \
  math_toolbox.cpp \
  shift_alpha_lock_view.cpp \
  suspend_timer.cpp \
  title_bar_view.cpp \
  variable_box_controller.cpp \
  variable_box_empty_controller.cpp \
)

snapshots_declaration = $(foreach i,$(apps),$(i)::Snapshot m_snapshot$(subst :,,$(i))Snapshot;)
apps_declaration = $(foreach i,$(apps),$(i) m_$(subst :,,$(i));)
snapshots_construction = $(foreach i,$(apps),,m_snapshot$(subst :,,$(i))Snapshot())
snapshots_list = $(foreach i,$(apps),,&m_snapshot$(subst :,,$(i))Snapshot)
snapshots_count = $(words $(apps))
snapshot_includes = $(foreach i,$(app_headers),-include $(i) )
epsilon_app_names = '$(foreach i,${EPSILON_APPS},"$(i)", )'

$(call object_for,apps/apps_container_storage.cpp apps/main.cpp): CXXFLAGS += $(snapshot_includes) -DAPPS_CONTAINER_APPS_DECLARATION="$(apps_declaration)" -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(snapshots_construction)" -DAPPS_CONTAINER_SNAPSHOT_LIST="$(snapshots_list)" -DAPPS_CONTAINER_SNAPSHOT_COUNT=$(snapshots_count) -DEPSILON_APPS_NAMES=$(epsilon_app_names)



# I18n file generation

# The header is refered to as <apps/i18n.h> so make sure it's findable this way
SFLAGS += -I$(build_dir)

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

$(eval $(call rule_for, \
  I18N, \
  apps/i18n.cpp, \
  $(i18n_files), \
  $$(PYTHON) apps/i18n.py --header $$(subst .cpp,.h,$$@) --implementation $$@ --locales $$(EPSILON_I18N) --files $$^ \
))

# Make sure apps/i18n.o is built
# The extension added to app_src doesn't really matter since it'll be processed
# by the object_for function.
app_src += apps/i18n.py

$(build_dir)/apps/i18n.h: $(build_dir)/apps/i18n.cpp

$(call object_for,$(app_src)): $(build_dir)/apps/i18n.h

# Handle PNG files

$(eval $(call depends_on_image,apps/title_bar_view.cpp,apps/exam_icon.png))


# Tracking which source file uses which image is painful. But we need to ensure
# that a .png file has been inlined before building any source file that uses
# said image (because it will expect the ".h" file to be there).
# As a shortcut, we simply say that every app file depends on every image. In
# practice, this forces all the images to be before the app.

#app_image_objs := $(build_dir)/$(app_images:.png=.o)
#.SECONDARY: $(build_dir)/$(app_images:.png=.cpp)
#$(app_objs): $(app_image_objs)
#app_image_src := $(app_images:.png=.cpp)

#epsilon.$(EXE): $(app_objs) $(app_image_objs)
#app_objs = $(call object_for,$(app_src))

#epsilon.$(EXE): $(app_objs)

src += $(app_src)
#$(app_image_src)

TO_REMOVE := apps/main.o apps/i18n.o
TMP := $(app_objs) $(app_image_objs)
VAR := $(filter-out $(TO_REMOVE), $(TMP))
test.$(EXE): $(VAR)
