SOURCES_apps += $(addprefix $(PATH_apps)/statistics/,$(addsuffix :-test, \
  stats/calculation_controller.cpp \
  stats/calculation_selectable_table_view.cpp \
  graph/box_banner_view.cpp \
  graph/box_controller.cpp \
  graph/box_parameter_controller.cpp \
  graph/box_range.cpp \
  graph/box_view.cpp \
  graph/chevrons.cpp \
  graph/data_view.cpp \
  graph/data_view_controller.cpp \
  graph/frequency_controller.cpp \
  graph/graph_type_controller.cpp \
  graph/graph_view_model.cpp \
  graph/histogram_banner_view.cpp \
  graph/histogram_cell.cpp \
	graph/histogram_list_controller.cpp \
  graph/histogram_main_view.cpp \
  graph/histogram_main_controller.cpp \
  graph/histogram_parameter_controller.cpp \
  graph/histogram_range.cpp \
  graph/histogram_view.cpp \
  graph/multiple_boxes_view.cpp \
  graph/multiple_data_view.cpp \
  graph/normal_probability_controller.cpp \
  graph/plot_banner_view.cpp \
  graph/plot_controller.cpp \
  graph/plot_curve_view.cpp \
  graph/plot_range.cpp \
  graph/plot_view.cpp \
  data/store_controller.cpp \
  data/store_parameter_controller.cpp \
  app.cpp \
  stat_icon.png \
  box_icon.png \
  histogram_icon.png \
  cumulative_icon.png \
  statistics_normal_icon.png \
) \
  store.cpp \
  test/store.cpp:+test \
)

$(call depends_on_image,apps/statistics/app.cpp,apps/statistics/stat_icon.png)

$(call depends_on_transparent_image, \
apps/statistics/graph/graph_view_model.cpp, \
$(addprefix apps/statistics/, \
  box_icon.png \
  histogram_icon.png \
  cumulative_icon.png \
  statistics_normal_icon.png \
))

$(call i18n_with_universal,statistics/i18n/base)
