snapshots += Sequence::App::Snapshot
snapshot_headers += apps/sequence/app.h

app_objs += $(addprefix apps/sequence/,\
  app.o\
  graph/banner_view.o\
  graph/curve_parameter_controller.o\
  graph/curve_view_range.o\
  graph/go_to_parameter_controller.o\
  graph/graph_controller.o\
  graph/graph_view.o\
  graph/term_sum_controller.o\
  list/list_controller.o\
  list/list_parameter_controller.o\
  list/sequence_toolbox.o\
  list/type_parameter_controller.o\
  values/interval_parameter_controller.o\
  values/values_controller.o\
  cache_context.o\
  sequence.o\
  sequence_context.o\
  sequence_store.o\
  sequence_title_cell.o\
)

i18n_files += $(addprefix apps/sequence/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests += $(addprefix apps/sequence/test/,\
  sequence.cpp\
)
test_objs += $(addprefix apps/sequence/, sequence.o sequence_store.o sequence_context.o cache_context.o)
test_objs += $(addprefix apps/shared/, function.o function_store.o)

app_images += apps/sequence/sequence_icon.png
