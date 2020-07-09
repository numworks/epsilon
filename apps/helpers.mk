# i18n helpers

# Should be called as: i18n_for_locale basename locale
define i18n_for_locale
$(addprefix apps/, $(addprefix $(1), $(addprefix ., $(addsuffix .i18n,$(2)))))
endef

# Should be called as: i18n_without_universal_for basename
# Adds the basename.**.i18n for all locales in EPSILON_I18N
define i18n_without_universal_for
$(foreach LOCALE,$(EPSILON_I18N),$(call i18n_for_locale, $(1), $(LOCALE)))
endef

# Should be called as: i18n_with_universal_for basename
# Adds basename.**.i18n for all EPSILON_I18N locales + basename.universal.i18n
define i18n_with_universal_for
$(call i18n_without_universal_for,$(1))
$(call i18n_for_locale,$(1),universal)
endef
