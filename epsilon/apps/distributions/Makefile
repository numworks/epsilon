SOURCES_apps += $(addprefix $(PATH_apps)/distributions/,$(addsuffix :-test, \
  app.cpp \
  controllers/calculation_cell.cpp \
  controllers/calculation_controller.cpp \
  controllers/calculation_popup_data_source.cpp \
  controllers/distribution_controller.cpp \
  controllers/distribution_curve_view.cpp \
  controllers/parameters_controller.cpp \
) \
  models/calculation/calculation.cpp \
  models/calculation/discrete_calculation.cpp \
  models/calculation/finite_integral_calculation.cpp \
  models/calculation/left_integral_calculation.cpp \
  models/calculation/right_integral_calculation.cpp \
  models/distribution/chi_squared_distribution.cpp \
  models/distribution/distribution.cpp \
  models/distribution/fisher_distribution.cpp \
  models/distribution/hypergeometric_distribution.cpp \
  models/distribution/student_distribution.cpp \
  models/distribution/exponential_distribution.cpp \
  models/distribution/normal_distribution.cpp \
  models/distribution/poisson_distribution.cpp \
  models/distribution/uniform_distribution.cpp \
  test/distributions_results.cpp:+test \
  test/distributions_parameters.cpp:+test \
)

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/app.cpp,$(PATH_apps)/icons/light/distributions_light_icon.png,$(PATH_apps)/icons/dark/distributions_dark_icon.png,:-test)

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/binomial_light_icon.png,$(PATH_apps)/distributions/images/dark/binomial_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/chi_squared_light_icon.png,$(PATH_apps)/distributions/images/dark/chi_squared_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/exponential_light_icon.png,$(PATH_apps)/distributions/images/dark/exponential_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/fisher_light_icon.png,$(PATH_apps)/distributions/images/dark/fisher_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/geometric_light_icon.png,$(PATH_apps)/distributions/images/dark/geometric_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/hypergeometric_light_icon.png,$(PATH_apps)/distributions/images/dark/hypergeometric_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/normal_light_icon.png,$(PATH_apps)/distributions/images/dark/normal_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/poisson_light_icon.png,$(PATH_apps)/distributions/images/dark/poisson_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/student_light_icon.png,$(PATH_apps)/distributions/images/dark/student_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/distribution_controller.cpp,$(PATH_apps)/distributions/images/light/uniform_light_icon.png,$(PATH_apps)/distributions/images/dark/uniform_dark_icon.png,:-test)

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/calculation_popup_data_source.cpp,$(PATH_apps)/distributions/images/light/calculation1_light_icon.png,$(PATH_apps)/distributions/images/dark/calculation1_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/calculation_popup_data_source.cpp,$(PATH_apps)/distributions/images/light/calculation2_light_icon.png,$(PATH_apps)/distributions/images/dark/calculation2_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/calculation_popup_data_source.cpp,$(PATH_apps)/distributions/images/light/calculation3_light_icon.png,$(PATH_apps)/distributions/images/dark/calculation3_dark_icon.png,:-test)
$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/distributions/controllers/calculation_popup_data_source.cpp,$(PATH_apps)/distributions/images/light/calculation4_light_icon.png,$(PATH_apps)/distributions/images/dark/calculation4_dark_icon.png,:-test)

$(call i18n_with_universal,distributions/i18n/base)
