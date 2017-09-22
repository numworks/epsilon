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
  transform/integer_addition_transform.o \
  transform/merge_addition_transform.o \
)
