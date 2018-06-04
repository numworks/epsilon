snapshots += Solver::App::Snapshot
snapshot_headers += apps/solver/app.h

app_objs += $(addprefix apps/solver/,\
  app.o\
  equation_models_parameter_controller.o\
  equation.o\
  equation_store.o\
  interval_controller.o\
  list_controller.o\
  solutions_controller.o\
)

i18n_files += $(addprefix apps/solver/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests += $(addprefix apps/solver/test/,\
  equation_store.cpp\
)
test_objs += $(addprefix apps/solver/, equation.o equation_store.o)
test_objs += $(addprefix apps/shared/, expression_model.o  expression_model_store.o)

app_images += apps/solver/solver_icon.png
