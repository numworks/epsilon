SOURCES_apps += $(addprefix $(PATH_apps)/distributions/, \
  app.cpp \
$(addprefix models/calculation/, \
  calculation.cpp \
  discrete_calculation.cpp \
  left_integral_calculation.cpp \
  right_integral_calculation.cpp \
  finite_integral_calculation.cpp \
) \
$(addprefix models/distribution/, \
  binomial_distribution.cpp \
  chi_squared_distribution.cpp \
  distribution.cpp \
  fisher_distribution.cpp \
  geometric_distribution.cpp \
  hypergeometric_distribution.cpp \
  student_distribution.cpp \
  exponential_distribution.cpp \
  normal_distribution.cpp \
  poisson_distribution.cpp \
  uniform_distribution.cpp \
) \
$(addprefix probability/, \
  calculation_cell.cpp \
  calculation_controller.cpp \
  calculation_popup_data_source.cpp \
  distribution_controller.cpp \
  distribution_curve_view.cpp \
  parameters_controller.cpp \
) \
$(addprefix test/, \
  distributions_results.cpp:+test \
  distributions_parameters.cpp:+test \
) \
)

$(call depends_on_image,$(PATH_apps)/distributions/app.cpp,$(PATH_apps)/distributions/distributions_icon.png)

$(call depends_on_transparent_image, \
$(PATH_apps)/distributions/probability/distribution_controller.cpp, \
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
$(addprefix $(PATH_apps)/distributions/probability/, \
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
