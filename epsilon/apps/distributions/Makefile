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

SOURCES_apps += $(call generated_sources_for,$(addprefix $(PATH_apps)/,$(addsuffix :-test, \
  icons/light/distributions_light_icon.png \
  icons/dark/distributions_dark_icon.png \
  distributions/images/light/binomial_light_icon.png \
  distributions/images/light/chi_squared_light_icon.png \
  distributions/images/light/exponential_light_icon.png \
  distributions/images/light/fisher_light_icon.png \
  distributions/images/light/geometric_light_icon.png \
  distributions/images/light/hypergeometric_light_icon.png \
  distributions/images/light/normal_light_icon.png \
  distributions/images/light/poisson_light_icon.png \
  distributions/images/light/student_light_icon.png \
  distributions/images/light/uniform_light_icon.png \
  distributions/images/light/calculation1_light_icon.png \
  distributions/images/light/calculation2_light_icon.png \
  distributions/images/light/calculation3_light_icon.png \
  distributions/images/light/calculation4_light_icon.png \
)))

$(call depends_on_transparent_image,$(PATH_apps)/distributions/app.cpp,$(PATH_apps)/icons/light/distributions_light_icon.png)
$(call depends_on_transparent_image,$(PATH_apps)/distributions/app.cpp,$(PATH_apps)/icons/dark/distributions_dark_icon.png)

$(call depends_on_transparent_image, \
$(PATH_apps)/distributions/controllers/distribution_controller.cpp, \
$(addprefix $(PATH_apps)/distributions/images/light/,\
  binomial_light_icon.png \
  chi_squared_light_icon.png \
  exponential_light_icon.png \
  fisher_light_icon.png \
  geometric_light_icon.png \
  hypergeometric_light_icon.png \
  normal_light_icon.png \
  poisson_light_icon.png \
  student_light_icon.png \
  uniform_light_icon.png \
))

$(call depends_on_transparent_image, \
$(addprefix $(PATH_apps)/distributions/controllers/, \
  calculation_controller.cpp \
  calculation_popup_data_source.cpp \
), \
$(addprefix $(PATH_apps)/distributions/images/light/,\
  calculation1_light_icon.png \
  calculation2_light_icon.png \
  calculation3_light_icon.png \
  calculation4_light_icon.png \
))

$(call i18n_with_universal,distributions/i18n/base)
