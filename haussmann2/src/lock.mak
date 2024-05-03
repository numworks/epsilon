_lockfile_name := locks.mak

# Private API

# lockfile_recipe, <flavored goal>
define lockfile_recipe
$(call _lockfile_recipe_helper,$(call name_for_flavored_target,$1),$(foreach m,$(MODULES_$(call name_for_flavored_target,$1)),$(call name_for_flavored_target,$m)))
endef

# Helpers

# _lockfile_recipe_helper, <flavorless goal>, <flavorless modules>
define _lockfile_recipe_helper
$(QUIET) echo 'LOCKS_$1 = $2' > $(_lockfile_name)
$(QUIET) $(foreach m,$2,echo 'VERSION_$m_FOR_$1 = $(VERSION_$m)' >> $(_lockfile_name);)
endef
