apps += Solver::App
app_headers += apps/solver/app.h

app_solver_test_src = $(addprefix apps/solver/,\
  equation.cpp \
  equation_store.cpp \
)

app_solver_src = $(addprefix apps/solver/,\
  app.cpp \
  equation_models_parameter_controller.cpp \
  equation_list_view.cpp \
  interval_controller.cpp \
  list_controller.cpp \
  solutions_controller.cpp \
)

app_solver_src += $(app_solver_test_src)
apps_src += $(app_solver_src)

i18n_files += $(addprefix apps/solver/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests_src += $(addprefix apps/solver/test/,\
  equation_store.cpp \
  helpers.cpp \
)

$(eval $(call depends_on_image,apps/solver/app.cpp,apps/solver/solver_icon.png))
