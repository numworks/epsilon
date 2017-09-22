include poincare/src/simplification/rulegen/Makefile

dir=poincare/src/simplification

%.h: $(RULEGEN) %.prs
	@echo "RULEGEN $@"
	@$(RULEGEN) < $(word 2,$^) > $@

rulesets = demo_ruleset.h
expression_simplify.o: $(addprefix $(dir), $(rulesets))
products += $(addprefix $(dir), $(rulesets))

objs += $(addprefix $(dir)/,\
  expression_simplify.o \
  rule.o \
  selector/combination.o \
  selector/selector.o \
  selector/type_selector.o \
  transform/integer_addition_transform.o \
  transform/merge_addition_transform.o \
)
