SOURCES_apps += $(addprefix $(PATH_apps)/solver/,$(addsuffix :-test, \
  app.cpp \
  equation_list_view.cpp \
  equation_models_parameter_controller.cpp \
  interval_controller.cpp \
  list_controller.cpp \
  solutions_controller.cpp \
) \
  equation.cpp \
  equation_store.cpp \
  system_of_equations.cpp \
  test/equation_store.cpp:+test \
  test/helpers.cpp:+test \
  test/solving_range.cpp:+test \
)

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/solver/app.cpp,$(PATH_apps)/icons/light/solver_light_icon.png,$(PATH_apps)/icons/dark/solver_dark_icon.png,:-test)

$(call i18n_without_universal,solver/i18n/base)
