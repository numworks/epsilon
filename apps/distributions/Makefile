SOURCES_apps += $(addprefix $(PATH_apps)/distributions/,$(addsuffix :-test, \
  app.cpp \
  controllers/calculation_cell.cpp \
  controllers/calculation_controller.cpp \
  controllers/calculation_popup_data_source.cpp \
  controllers/distribution_controller.cpp \
  controllers/distribution_curve_view.cpp \
  controllers/parameters_controller.cpp \
  distributions_icon.png \
  images/binomial_icon.png \
  images/chi_squared_icon.png \
  images/exponential_icon.png \
  images/fisher_icon.png \
  images/geometric_icon.png \
  images/hypergeometric_icon.png \
  images/normal_icon.png \
  images/poisson_icon.png \
  images/student_icon.png \
  images/uniform_icon.png \
  images/calculation1_icon.png \
  images/calculation2_icon.png \
  images/calculation3_icon.png \
  images/calculation4_icon.png \
) \
  models/calculation/calculation.cpp \
  models/calculation/discrete_calculation.cpp \
  models/calculation/finite_integral_calculation.cpp \
  models/calculation/left_integral_calculation.cpp \
  models/calculation/right_integral_calculation.cpp \
  models/distribution/binomial_distribution.cpp \
  models/distribution/chi_squared_distribution.cpp \
  models/distribution/distribution.cpp \
  models/distribution/fisher_distribution.cpp \
  models/distribution/geometric_distribution.cpp \
  models/distribution/hypergeometric_distribution.cpp \
  models/distribution/student_distribution.cpp \
  models/distribution/exponential_distribution.cpp \
  models/distribution/normal_distribution.cpp \
  models/distribution/poisson_distribution.cpp \
  models/distribution/uniform_distribution.cpp \
  test/distributions_results.cpp:+test \
  test/distributions_parameters.cpp:+test \
)

$(call depends_on_image,$(PATH_apps)/distributions/app.cpp,$(PATH_apps)/distributions/distributions_icon.png)

$(call depends_on_transparent_image, \
$(PATH_apps)/distributions/controllers/distribution_controller.cpp, \
$(addprefix $(PATH_apps)/distributions/images/,\
  binomial_icon.png \
  chi_squared_icon.png \
  exponential_icon.png \
  fisher_icon.png \
  geometric_icon.png \
  hypergeometric_icon.png \
  normal_icon.png \
  poisson_icon.png \
  student_icon.png \
  uniform_icon.png \
))

$(call depends_on_transparent_image, \
$(addprefix $(PATH_apps)/distributions/controllers/, \
  calculation_controller.cpp \
  calculation_popup_data_source.cpp \
), \
$(addprefix $(PATH_apps)/distributions/images/,\
  calculation1_icon.png \
  calculation2_icon.png \
  calculation3_icon.png \
  calculation4_icon.png \
))

$(call i18n_with_universal,distributions/i18n/base)
