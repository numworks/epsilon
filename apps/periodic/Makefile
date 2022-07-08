apps += Periodic::App
app_headers += apps/periodic/app.h

app_periodic_src = $(addprefix apps/periodic/,\
  app.cpp \
  banner_view.cpp \
  coloring.cpp \
  elements_data_base.cpp \
  elements_view.cpp \
  elements_view_data_source.cpp \
  main_controller.cpp \
  single_element_view.cpp \
  table_layout.cpp \
)

apps_src += $(app_periodic_src)

i18n_files += apps/periodic/base.universal.i18n

$(eval $(call depends_on_image,apps/periodic/app.cpp,apps/settings/settings_icon.png))
