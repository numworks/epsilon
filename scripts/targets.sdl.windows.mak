$(eval $(call rule_for, \
  WINDRES, %.o, %.rc, \
  $$(WINDRES) $$< -O coff -o $$@ \
))
