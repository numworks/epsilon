SOURCES_apps += $(addprefix $(PATH_apps)/finance/,$(addsuffix :-test, \
  app.cpp \
  interest_menu_controller.cpp \
  menu_controller.cpp \
  parameters_controller.cpp \
  result_controller.cpp \
) \
  data.cpp \
  test/finance.cpp:+test \
)

$(call depends_on_transparent_themed_image,apps,$(PATH_apps)/finance/app.cpp,$(PATH_apps)/icons/light/finance_light_icon.png,$(PATH_apps)/icons/dark/finance_dark_icon.png,:-test)

$(call i18n_with_universal,finance/i18n/base)
