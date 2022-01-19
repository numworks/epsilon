apps += Solver::App
app_headers += apps/solver/app.h

app_solver_test_src = $(addprefix apps/solver/,\
  model/data.cpp \
  model/equation.cpp \
  model/equation_store.cpp \
)

app_solver_src = $(addprefix apps/solver/,\
  app.cpp \
  gui/equation_list_view.cpp \
  gui/title_table_view.cpp \
  controllers/equation_models_parameter_controller.cpp \
  controllers/interval_controller.cpp \
  controllers/list_controller.cpp \
  controllers/menu_controller.cpp \
  controllers/solutions_controller.cpp \
  controllers/finance_menu_controller.cpp \
  controllers/simple_interest_controller.cpp \
  controllers/simple_interest_menu_controller.cpp \
  controllers/compound_interest_controller.cpp \
  controllers/compound_interest_menu_controller.cpp \
  controllers/finance_result_controller.cpp \
)

app_solver_src += $(app_solver_test_src)
apps_src += $(app_solver_src)

i18n_files += $(call i18n_with_universal_for,solver/translations/base)

tests_src += $(addprefix apps/solver/test/,\
  equation_store.cpp \
  finance.cpp \
  helpers.cpp \
)

# Image dependencies

$(eval $(call depends_on_image,apps/solver/app.cpp,apps/solver/solver_icon.png))

$(eval $(call depends_on_transparent_image,apps/solver/controllers/menu_controller.cpp,$(addprefix apps/solver/images/,\
  Equations.png \
  Finances.png \
)))
