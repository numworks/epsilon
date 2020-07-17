app_on_boarding_src = $(addprefix apps/on_boarding/,\
  app.cpp \
  logo_controller.cpp \
  logo_view.cpp \
  localization_controller.cpp \
  pop_up_controller.cpp \
  power_on_self_test.cpp \
)

apps_src += $(app_on_boarding_src)

i18n_files += $(call i18n_without_universal_for,on_boarding/base)

$(eval $(call depends_on_image,apps/on_boarding/logo_view.cpp,apps/on_boarding/logo_icon.png))
