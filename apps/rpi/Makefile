apps += Rpi::App
app_headers += apps/rpi/app.h

app_src += $(addprefix apps/rpi/,\
  app.cpp \
  rpi_controller.cpp \
  rpi_view.cpp \
)

app_images += apps/rpi/rpi_icon.png

i18n_files += $(addprefix apps/rpi/,\
	base.de.i18n\
	base.en.i18n\
	base.es.i18n\
	base.fr.i18n\
	base.pt.i18n\
)

$(eval $(call depends_on_image,apps/rpi/app.cpp,apps/rpi/rpi_icon.png))
