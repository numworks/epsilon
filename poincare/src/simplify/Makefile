#include poincare/src/simplify/rules_generation/Makefile

dir=poincare/src/simplify

$(dir)/rules.cpp: $(RULEGEN) $(dir)/rules.pr
	@echo "RULEGEN $@"
	@$(RULEGEN) < $(dir)/rules.pr > $@

products += $(dir)/rules.cpp

objs += $(addprefix $(dir)/,\
  combination.o \
  expression.o \
  rule.o \
  integer_addition.o \
  merge_addition.o \
  type_selector.o \
  selector.o \
)
