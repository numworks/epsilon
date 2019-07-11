apps += Calculation::App
app_headers += apps/calculation/app.h

app_calculation_src = $(addprefix apps/calculation/,\
  app.cpp \
  calculation.cpp \
  calculation_store.cpp \
  edit_expression_controller.cpp \
  expression_field.cpp \
  history_view_cell.cpp \
  history_controller.cpp \
  scrollable_expression_view.cpp \
  selectable_table_view.cpp \
)

app_src += $(app_calculation_src)

i18n_files += $(addprefix apps/calculation/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

tests_src += $(addprefix apps/calculation/test/,\
  calculation_store.cpp\
)

$(eval $(call depends_on_image,apps/calculation/app.cpp,apps/calculation/calculation_icon.png))
