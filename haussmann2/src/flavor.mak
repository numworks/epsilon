# Define a syntax to select specific elements of a list, using the function:
#   flavor_filter, <list with tastes>, <flavors>
#
# - "flavors" are filters used to chose which elements are selected
# - "tastes" are qualifiers affixed to elements of a list to specify how each
#   one interacts with flavors.
#
# The syntax for tastes is as follows:
# - elem:+flavor1 means the element is skipped if flavor1 IS NOT provided
# - elem:-flavor2 means the element is skipped if flavor1 IS provided
#
# Combination of tastes works as a logical AND for selection:
# - elem:+f1:-f2:+f3 will be skipped if f2 is present of if either f1 or f3 is
#   absent.
#
# Logical OR can be achieved by adding the element several times in the list
# with different tastes.

# e.g. "make flavor_test" should print "e1 e5"
.PHONY: flavor_test
flavor_test: flavor_test_list := e1 e2:-f1 e3:+f2 e4:+f1:-f3 e5:+f3:-f4
flavor_test:
	$(QUIET) echo '$(call flavor_filter,$(flavor_test_list),f1 f3)'

# flavor_filter, <list with tastes>, <flavors>
define flavor_filter
$(strip $(foreach e,$1,$(call _filter_element_with_tastes,$(subst :, :,$e),$2)))
endef

# tasteless_filter, <list with tastes>
define tasteless_filter
$(strip $(filter-out :%,$(subst :, :,$1)))
endef

# Helpers

# _filter_element_with_tastes, <element and tastes>, <flavors>
define _filter_element_with_tastes
$(call _filter_negative_tastes,\
	$(call _filter_positive_tastes,\
		$(firstword $1),\
		$(subst :+,,$(filter :+%,$1)),\
		$2),\
	$(subst :-,,$(filter :-%,$1)),\
	$2)
endef

# _filter_positive_tastes, <element>, <positive tastes>, <flavors>
define _filter_positive_tastes
$(if $(filter-out $3,$2),,$1)
endef
# _filter_negative_tastes, <element>, <negative tastes>, <flavors>
define _filter_negative_tastes
$(if $(filter $2,$3),,$1)
endef


