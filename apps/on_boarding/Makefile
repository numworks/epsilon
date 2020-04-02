app_on_boarding_src = $(addprefix apps/on_boarding/,\
  app.cpp \
  language_controller.cpp \
  logo_controller.cpp \
  logo_view.cpp \
  pop_up_controller.cpp \
  power_on_self_test.cpp \
)

apps_src += $(app_on_boarding_src)

i18n_files += $(addprefix apps/on_boarding/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

$(eval $(call depends_on_image,apps/on_boarding/logo_view.cpp,apps/on_boarding/logo_icon.png))
