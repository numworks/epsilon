#include poincare/src/simplify/rules_generation/Makefile

dir=poincare/src/simplification

$(dir)/rules.cpp: $(RULEGEN) $(dir)/rules.pr
	@echo "RULEGEN $@"
	@$(RULEGEN) < $(dir)/rules.pr > $@

products += $(dir)/rules.cpp

objs += $(addprefix $(dir)/,\
  expression_simplify.o \
  rule.o \
  selector/combination.o \
  selector/selector.o \
  selector/type_selector.o \
  transform/integer_addition_transform.o \
  transform/merge_addition_transform.o \
)
