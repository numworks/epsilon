apps += Finance::App
app_headers += apps/finance/app.h

app_finance_test_src = $(addprefix apps/finance/,\
  data.cpp \
)

app_finance_src = $(addprefix apps/finance/,\
  app.cpp \
  menu_controller.cpp \
  interest_controller.cpp \
  interest_menu_controller.cpp \
  result_controller.cpp \
)

app_finance_src += $(app_finance_test_src)
apps_src += $(app_finance_src)

i18n_files += $(call i18n_with_universal_for,finance/base)

tests_src += $(addprefix apps/finance/test/,\
  finance.cpp \
)

$(eval $(call depends_on_image,apps/finance/app.cpp,apps/finance/finance_icon.png))
