SOURCES_apps += $(addprefix $(PATH_apps)/finance/, \
  app.cpp \
  data.cpp \
  interest_menu_controller.cpp \
  menu_controller.cpp \
  parameters_controller.cpp \
  result_controller.cpp \
  test/finance.cpp:+test \
  finance_icon.png \
)

$(call depends_on_image,$(PATH_apps)/finance/app.cpp,$(PATH_apps)/finance/finance_icon.png)

$(call i18n_with_universal,finance/i18n/base)
