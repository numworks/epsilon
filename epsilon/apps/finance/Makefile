SOURCES_apps += $(addprefix $(PATH_apps)/finance/,$(addsuffix :-test, \
  app.cpp \
  interest_menu_controller.cpp \
  menu_controller.cpp \
  parameters_controller.cpp \
  result_controller.cpp \
  finance_icon.png \
) \
  data.cpp \
  test/finance.cpp:+test \
)

$(call depends_on_image,$(PATH_apps)/finance/app.cpp,$(PATH_apps)/finance/finance_icon.png)

$(call i18n_with_universal,finance/i18n/base)
