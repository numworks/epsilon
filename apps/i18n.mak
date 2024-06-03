_language_preferences = $(PATH_apps)/language_preferences.csv
_supported_locales := en fr nl pt it de es
EPSILON_I18N ?= $(_supported_locales)

_country_preferences = $(PATH_apps)/country_preferences.csv
_supported_countries := WW CA DE ES FR GB IT NL PT US
EPSILON_COUNTRIES ?= $(_supported_countries)

# _apps_i18n will be filled later by the apps
_apps_i18n =

# i18n_for_locale, <basename>, <locales>
define i18n_for_locale
$(eval _apps_i18n += $(patsubst %,$(PATH_apps)/$(strip $1).%.i18n,$(strip $2)))
endef

# i18n_without_universal, <basename>
define i18n_without_universal
$(foreach l,$(EPSILON_I18N),$(call i18n_for_locale,$1,$l))
endef

# i18n_with_universal, <basename>
define i18n_with_universal
$(call i18n_without_universal,$1) $(call i18n_for_locale,$1,universal)
endef

$(call assert_defined,KANDINSKY_codepoints)

$(OUTPUT_DIRECTORY)/$(PATH_apps)/i18n.h: $(PATH_apps)/i18n.py $$(_apps_i18n) $(_language_preferences) $(_country_preferences) | $$(@D)/.
	$(QUIET) $(PYTHON) $< \
		--header $@ \
		--implementation $(basename $@).cpp \
		--locales $(EPSILON_I18N) \
		--supported-locales $(_supported_locales) \
		--countries $(EPSILON_COUNTRIES) \
		--supported-countries $(_supported_countries) \
		--codepoints $(KANDINSKY_codepoints) \
		--languagepreferences $(_language_preferences) \
		--countrypreferences $(_country_preferences) \
		--files $(_apps_i18n) \
		$(if $(filter 1,$(I18N_COMPRESS)),--compress,)

# FIXME PRIORITY_TARGETS_<module> is used if files in other modules might depend
# on those targets, but apps should not be upstream of anything.
PRIORITY_TARGETS_apps += $(OUTPUT_DIRECTORY)/$(PATH_apps)/i18n.h
