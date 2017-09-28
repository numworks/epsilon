snapshots += Calculation::App::Snapshot
snapshot_headers += apps/calculation/app.h

app_objs += $(addprefix apps/calculation/,\
  app.o\
  calculation.o\
  calculation_store.o\
  edit_expression_controller.o\
  history_view_cell.o\
  history_controller.o\
  local_context.o\
  scrollable_expression_view.o\
  selectable_table_view.o\
  text_field.o\
)

i18n_files += $(addprefix apps/calculation/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests += $(addprefix apps/calculation/test/,\
  calculation_store.cpp\
)
test_objs += $(addprefix apps/calculation/, calculation.o calculation_store.o)

app_images += apps/calculation/calculation_icon.png

