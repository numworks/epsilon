apps += Omega::App
app_headers += apps/omega/app.h

app_src += $(addprefix apps/omega/,\
  app.cpp \
  omega_controller.cpp \
  omega_view.cpp \
)

app_images += apps/omega/omega_icon.png

i18n_files += $(addprefix apps/omega/,\
	base.de.i18n\
	base.en.i18n\
	base.es.i18n\
	base.fr.i18n\
	base.pt.i18n\
	base.hu.i18n\
	base.universal.i18n\
)

$(eval $(call depends_on_image,apps/omega/app.cpp,apps/omega/omega_icon.png))