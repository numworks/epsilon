$(call create_module,ion,1,)

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

include $(PATH_ion)/shared.$(PLATFORM_TYPE).mak
