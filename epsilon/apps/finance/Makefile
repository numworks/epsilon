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

SOURCES_apps += $(call generated_sources_for,$(addprefix $(PATH_apps)/finance/, \
  finance_light_icon.png:-test \
  finance_dark_icon.png:-test \
))

$(call depends_on_transparent_image,$(PATH_apps)/finance/app.cpp,$(PATH_apps)/finance/finance_light_icon.png)
$(call depends_on_transparent_image,$(PATH_apps)/finance/app.cpp,$(PATH_apps)/finance/finance_dark_icon.png)

$(call i18n_with_universal,finance/i18n/base)
