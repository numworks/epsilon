SOURCES_apps += $(addprefix $(PATH_apps)/statistics/,$(addsuffix :-test, \
  app.cpp \
  shared/statistics_type_controller.cpp \
  shared/statistics_type_view_model.cpp \
  $(addprefix categorical/, \
    data/column_parameter_controller.cpp \
    data/rf_column_parameter_controller.cpp \
    data/row_parameter_controller.cpp \
    data/store.cpp \
    data/store_controller.cpp \
    data/table_data_source.cpp \
    graph/bar_banner_view.cpp \
    graph/bar_graph_controller.cpp \
    graph/bar_graph_view.cpp \
    graph/graph_view_model.cpp \
    graph/pie_banner_view.cpp \
    graph/pie_displayed_data_controller.cpp \
    graph/pie_graph_controller.cpp \
    graph/pie_graph_view.cpp \
  ) \
  $(addprefix quantitative/, \
    data/store_controller.cpp \
    data/store_parameter_controller.cpp \
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
    graph/histogram_main_controller.cpp \
    graph/histogram_main_view.cpp \
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
    stats/calculation_controller.cpp \
    stats/calculation_selectable_table_view.cpp \
  ) \
) \
quantitative/store.cpp \
quantitative/test/store.cpp:+test \
)

SOURCES_apps += $(call generated_sources_for, \
  $(addprefix $(PATH_apps)/,$(addsuffix :-test, \
  statistics/assets/light/bar_light_icon.png \
  statistics/assets/light/pie_light_icon.png \
  statistics/assets/light/box_light_icon.png \
  statistics/assets/light/histogram_light_icon.png \
  statistics/assets/light/cumulative_light_icon.png \
  statistics/assets/light/statistics_normal_light_icon.png \
  statistics/assets/dark/bar_dark_icon.png \
  statistics/assets/dark/pie_dark_icon.png \
  statistics/assets/dark/box_dark_icon.png \
  statistics/assets/dark/histogram_dark_icon.png \
  statistics/assets/dark/cumulative_dark_icon.png \
  statistics/assets/dark/statistics_normal_dark_icon.png \
)))

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/statistics/app.cpp,$(PATH_apps)/icons/light/stat_light_icon.png,$(PATH_apps)/icons/dark/stat_dark_icon.png)

$(call depends_on_transparent_image, \
apps/statistics/quantitative/graph/graph_view_model.cpp, \
$(addprefix apps/statistics/assets/light/, \
  box_light_icon.png \
  histogram_light_icon.png \
  cumulative_light_icon.png \
  statistics_normal_light_icon.png \
) \
$(addprefix apps/statistics/assets/dark/, \
  box_dark_icon.png \
  histogram_dark_icon.png \
  cumulative_dark_icon.png \
  statistics_normal_dark_icon.png \
))

$(call depends_on_transparent_image, \
apps/statistics/categorical/graph/graph_view_model.cpp, \
$(addprefix apps/statistics/assets/light/, \
  bar_light_icon.png \
  pie_light_icon.png \
) \
$(addprefix apps/statistics/assets/dark/, \
  bar_dark_icon.png \
  pie_dark_icon.png \
))

$(call i18n_with_universal,statistics/i18n/base)
