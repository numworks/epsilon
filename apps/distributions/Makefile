apps += Distributions::App
app_headers += apps/distributions/app.h

app_distributions_test_src += $(addprefix apps/distributions/models/distribution/,\
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
)

app_distributions_src = $(addprefix apps/distributions/,\
  app.cpp \
)

app_distributions_test_src += $(addprefix apps/distributions/models/calculation/,\
  calculation.cpp \
  discrete_calculation.cpp \
  left_integral_calculation.cpp \
  right_integral_calculation.cpp \
  finite_integral_calculation.cpp \
)

app_distributions_src += $(addprefix apps/distributions/probability/,\
  calculation_cell.cpp \
  calculation_controller.cpp \
  calculation_popup_data_source.cpp \
  distribution_cell.cpp \
  distribution_controller.cpp \
  distribution_curve_view.cpp \
  parameters_controller.cpp \
)

app_distributions_src += $(app_distributions_test_src)
apps_src += $(app_distributions_src)

i18n_files += $(call i18n_with_universal_for,distributions/i18n/base)

tests_src += $(addprefix apps/distributions/test/,\
  distributions.cpp \
)

# Allow #include "distributions/..."
sources = apps/apps_container.cpp apps/apps_container_storage.cpp apps/init.cpp apps/main.cpp $(app_distributions_src) $(app_distributions_test_src) $(tests_src)
$(call object_for,$(sources)): SFLAGS += -Iapps

# Image dependencies

$(eval $(call depends_on_image,apps/distributions/app.cpp,apps/distributions/distributions_icon.png))

$(eval $(call depends_on_transparent_image,apps/distributions/probability/distribution_controller.cpp,$(addprefix apps/distributions/images/,\
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
)))

$(eval $(call depends_on_transparent_image,apps/distributions/probability/calculation_controller.cpp \
 apps/distributions/probability/calculation_popup_data_source.cpp,$(addprefix apps/distributions/images/,\
  calculation1_icon.png \
  calculation2_icon.png \
  calculation3_icon.png \
  calculation4_icon.png \
)))
