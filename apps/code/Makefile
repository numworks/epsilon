snapshots += Code::App::Snapshot
snapshot_headers += apps/code/app.h

app_objs += $(addprefix apps/code/,\
  app.o\
  console_controller.o\
  console_edit_cell.o\
  console_line_cell.o\
  console_store.o\
  editor_controller.o\
  executor_controller.o\
  menu_controller.o\
  program.o\
)

i18n_files += $(addprefix apps/code/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests += $(addprefix apps/code/test/,\
  mpprint.cpp\
)
app_images += apps/code/code_icon.png
