apps += CalculationJunior::App
app_headers += apps/calculation_junior/app.h

app_calculation_junior_src = $(addprefix apps/calculation_junior/,\
  app.cpp \
  main_controller.cpp \
  expression_view.cpp \
  expression_field_delegate_app.cpp \
  layout_field.cpp \
)

apps_src += $(app_calculation_junior_src)

$(eval $(call depends_on_image,apps/calculation_junior/app.cpp,apps/calculation_junior/calculation_junior_icon.png))
