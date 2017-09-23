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
  selector/type_selector.o \
  transform/fraction_transform.o \
  transform/subtraction_transform.o \
  transform/integer_addition_transform.o \
  transform/merge_dynamic_hierarchy_transform.o \
)

