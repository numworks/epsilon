include poincare/src/simplification/rulegen/Makefile

prefix = poincare/src/simplification

%.h: $(RULEGEN) %.prs
	@echo "RULEGEN $@"
	@$(RULEGEN) < $(word 2,$^) > $@

rulesets = $(addprefix $(prefix)/,\
  demo_ruleset.h \
)
$(prefix)/expression_simplify.o: $(rulesets)
products += $(rulesets)

objs += $(addprefix $(prefix)/,\
  expression_simplify.o \
  rule.o \
  selector/combination.o \
  selector/selector.o \
  selector/type_and_identifier_selector.o \
  selector/type_selector.o \
  transform/division_transform.o \
  transform/remove_child_transform.o \
  transform/remove_parenthesis_transform.o \
  transform/subtraction_transform.o \
  transform/integer_dynamic_hierarchy_transform.o \
  transform/integer_power_transform.o \
  transform/merge_dynamic_hierarchy_transform.o \
  transform/opposite_transform.o \
  transform/rational_addition_transform.o \
  transform/rational_reduction_transform.o \
  transform/rational_power_transform.o \
  transform/replace_with_transform.o \
  transform/root_transform.o \
)

