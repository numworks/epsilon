apps += Solver::App
app_headers += apps/solver/app.h

app_solver_test_src = $(addprefix apps/solver/,\
  equations/equation.cpp \
  equations/equation_store.cpp \
)

app_solver_src = $(addprefix apps/solver/,\
  app.cpp \
  equations/equation_list_view.cpp \
  equations/equation_models_parameter_controller.cpp \
  equations/interval_controller.cpp \
  equations/list_controller.cpp \
  equations/solutions_controller.cpp \
)

app_solver_src += $(app_solver_test_src)
apps_src += $(app_solver_src)

i18n_files += $(call i18n_without_universal_for,solver/base)

tests_src += $(addprefix apps/solver/test/,\
  equation_store.cpp \
  helpers.cpp \
)

# Image dependencies

$(eval $(call depends_on_image,apps/solver/app.cpp,apps/solver/solver_icon.png))
