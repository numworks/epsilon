apps += Finance::App
app_headers += apps/finance/app.h

app_finance_src = $(addprefix apps/finance/,\
  app.cpp \
)

apps_src += $(app_finance_src)

i18n_files += apps/finance/base.universal.i18n

$(eval $(call depends_on_image,apps/finance/app.cpp,apps/finance/finance_icon_temp.png))
