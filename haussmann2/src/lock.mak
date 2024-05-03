LOCKFILE_NAME := locks.mak

# Private API

# check_locks, <module>
define check_locks
$(foreach m,$(LOCKS_$1),$(if $(filter $(VERSION_$m),$(VERSION_$m_FOR_$1)),,\
	$(error Error: using $m version $(VERSION_$m) but $1 expects $(VERSION_$m_FOR_$1))))
endef

# lockfile_recipe, <flavored goal>
define lockfile_recipe
$(call _lockfile_recipe_helper,$(call name_for_flavored_target,$1),$(foreach m,$(MODULES_$(call name_for_flavored_target,$1)),$(call name_for_flavored_target,$m)))
endef

# Helpers

# _lockfile_recipe_helper, <flavorless goal>, <flavorless modules>
define _lockfile_recipe_helper
$(QUIET) echo 'LOCKS_$1 = $2' > $(LOCKFILE_NAME)
$(QUIET) $(foreach m,$2,echo 'VERSION_$m_FOR_$1 = $(VERSION_$m)' >> $(LOCKFILE_NAME);)
endef
