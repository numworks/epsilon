apps += Statistics::App
app_headers += apps/statistics/app.h

app_statistics_test_src = $(addprefix apps/statistics/,\
  store.cpp \
)

app_statistics_src = $(addprefix apps/statistics/,\
  app.cpp \
  box_axis_view.cpp \
  box_banner_view.cpp \
  box_controller.cpp \
  box_range.cpp \
  box_view.cpp \
  calculation_controller.cpp \
  calculation_selectable_table_view.cpp \
  histogram_banner_view.cpp \
  histogram_controller.cpp \
  histogram_parameter_controller.cpp \
  histogram_view.cpp \
  multiple_boxes_view.cpp \
  multiple_data_view.cpp \
  multiple_data_view_controller.cpp \
  multiple_histograms_view.cpp \
  statistics_context.cpp \
  store_controller.cpp \
)

app_statistics_src += $(app_statistics_test_src)
apps_src += $(app_statistics_src)

i18n_files += $(addprefix apps/statistics/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests_src += $(addprefix apps/statistics/test/,\
  store.cpp\
)

$(eval $(call depends_on_image,apps/statistics/app.cpp,apps/statistics/stat_icon.png))
