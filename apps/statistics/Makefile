SOURCES_apps += $(addprefix $(PATH_apps)/statistics/, \
  store.cpp \
  app.cpp \
$(addprefix stats/, \
  calculation_controller.cpp \
  calculation_selectable_table_view.cpp \
) \
$(addprefix graph/, \
  box_banner_view.cpp \
  box_controller.cpp \
  box_parameter_controller.cpp \
  box_range.cpp \
  box_view.cpp \
  chevrons.cpp \
  data_view.cpp \
  data_view_controller.cpp \
  frequency_controller.cpp \
  graph_type_controller.cpp \
  graph_view_model.cpp \
  histogram_banner_view.cpp \
  histogram_controller.cpp \
  histogram_parameter_controller.cpp \
  histogram_range.cpp \
  histogram_view.cpp \
  multiple_boxes_view.cpp \
  multiple_data_view.cpp \
  multiple_histograms_view.cpp \
  normal_probability_controller.cpp \
  plot_banner_view.cpp \
  plot_controller.cpp \
  plot_curve_view.cpp \
  plot_range.cpp \
  plot_view.cpp \
) \
$(addprefix data/, \
  store_controller.cpp \
  store_parameter_controller.cpp \
) \
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
