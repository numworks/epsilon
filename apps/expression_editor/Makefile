snapshots += ExpressionEditor::App::Snapshot
snapshot_headers += apps/expression_editor/app.h

app_objs += $(addprefix apps/expression_editor/,\
  app.o\
  controller.o\
  expression_and_layout.o\
  expression_editor_view.o\
  expression_view_with_cursor.o\
  scrollable_expression_view_with_cursor.o\
)

i18n_files += $(addprefix apps/expression_editor/,\
  base.universal.i18n\
)

tests += $(addprefix apps/expression_editor/test/,\
  dummy_test.cpp\
)

test_objs += $(addprefix apps/expression_editor/,\
  controller.o\
  expression_editor_view.o\
  expression_view_with_cursor.o\
  scrollable_expression_view_with_cursor.o\
)
